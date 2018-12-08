#include <algorithm>
#include <ctime>
#include <iostream>
#include <queue>
#include <numeric>
#include <vector>

#include "mpi.h"

class Solver
{
public:
  int id;
  int nb_spots;
  int nb_colors;
  std::vector<int> secret;
  std::vector<std::vector<int>> possible_guesses;

  Solver(int id, int nb_spots, int nb_colors)
  {
    this->id = id;
    this->nb_spots = nb_spots;
    this->nb_colors = nb_colors;
    this->possible_guesses = solver_possible_guess();
  }

  Solver(int nb_spots, int nb_colors)
  {
    this->nb_spots = nb_spots;
    this->nb_colors = nb_colors;
    this->secret = new_secret(nb_spots, nb_colors);
  }

  //Utility method
  void print_possible_guesses()
  {
    if (!(possible_guesses.empty()))
    {
      for (int i=0; i < possible_guesses.size(); ++i)
      {
        for (int j=0; j < nb_spots; ++j)
          std::cout << possible_guesses[i][j] << " ";
        std::cout << "\n";
      }
    }
    else
      std::cout << "No possible guess left\n";
  }

  int factorial(int n)
  {
    int result = 1;
    while (n > 1)
      result *= n--;
    return result;
  }

  //Find the set of all possible solutions.
  std::vector<std::vector<int>> all_possible_guess()
  {
    std::vector<std::vector<int>> all_guess;
    std::vector<int> possible_guess;
    std::vector<int> permu(nb_colors);
    std::iota(permu.begin(), permu.end(), 0);
    int repeat = factorial(nb_colors-nb_spots);
    do
    {
      for (int i=0; i < nb_spots; i++)
      {
        possible_guess.push_back(permu[i]);
      }
      all_guess.push_back(possible_guess);
      possible_guess.clear();
      for (int i=1; i != repeat; i++)
        next_permutation(permu.begin(), permu.end());
    } while (std::next_permutation(permu.begin(), permu.end()));
    return all_guess;
  }

  //Partition the possible solution among the solvers using their ids.
  std::vector<std::vector<int>> solver_possible_guess()
  {
    std::vector<std::vector<int>> solver_guess;
    std::vector<int> guess;
    std::vector<std::vector<int>> all_guess = all_possible_guess();
    for (int i=0; i < all_guess.size(); ++i)
    {
      if (all_guess[i][0] == id)
      {
        for(int j=0; j < nb_spots; ++j)
          guess.push_back(all_guess[i][j]);
        solver_guess.push_back(guess);
        guess.clear();
      }
    }
    return solver_guess;
  }

  std::vector<int> give_next_guess()
  {
    std::vector<int> guess;
    if (!(possible_guesses.empty()))
    {
      for(int i=0; i < nb_spots; ++i)
        guess.push_back(possible_guesses[0][i]);
      possible_guesses.erase(std::remove(possible_guesses.begin(),
                             possible_guesses.end(), guess), possible_guesses.end());
    }
    return guess;
  }

  void update_possible_guesses(std::vector<int> old_guess, int perfect, int colors_only)
  {
    int tmp_perfect = perfect;
    int tmp_colors_only = colors_only;
    std::vector<int> new_guess;
    for (int i=0; i < possible_guesses.size(); ++i)
    {
      for (int j=0; j < nb_spots; ++j)
        new_guess.push_back(possible_guesses[i][j]);
      check_solution(old_guess, new_guess, &tmp_perfect, &tmp_colors_only);
      if ((tmp_perfect != perfect) || (tmp_colors_only != colors_only))
        possible_guesses.erase(std::remove(possible_guesses.begin(),
                               possible_guesses.end(), new_guess), possible_guesses.end());
      new_guess.clear();
    }
  }


  //Check if the guess is correct or not
  bool check_solution(std::vector<int> secret, std::vector<int> guess, int *perfect, int *colors_only)
  {
    int b = 1;
    *perfect = 0;
    *colors_only = 0;
    for (int i = 0; i < secret.size(); ++i)
    {
      if (secret[i] == guess[i])
        (*perfect)++;
      else
      {
        if (std::find(secret.begin(), secret.end(), guess[i]) != secret.end())
          (*colors_only)++;
        b = 0;
      }
    }
    return b;
  }

  //Give a new random system
  std::vector<int> new_secret(int nb_spots, int nb_colors)
  {
    int i = 0, new_color;
    std::vector<int> secret;
    while(i < nb_spots)
    {
      new_color = std::rand() % nb_colors;
      if (!(std::find(secret.begin(), secret.end(), new_color) != secret.end()))
      {
        secret.push_back(new_color);
        i++;
      }
    }
    return secret;
  }

};

/////////////////////

using namespace std;

void print_colors(vector<int> secret, int nb_spots, vector<string>  colors_names);
void print_colors(vector<int> secret, int nb_spots, vector<string>  colors_names,
                  int perfect, int colors_only);

//Print the colors
void print_colors(vector<int> secret, int nb_spots, vector<string> colors_names)
{
  for (int i = 0; i < nb_spots; ++i)
    cout << colors_names[secret[i]] << " ";
  cout << "\n";
}

//Print the colors and the perfect and color only values
void print_colors(vector<int> secret, int nb_spots, vector<string> colors_names,
                  int perfect, int colors_only)
{
  for (int i = 0; i < nb_spots; ++i)
    cout << colors_names[secret[i]] << " ";
  cout << "\nPerfect: " << perfect << " - Color Only: " << colors_only << "\n";
  cout << "\n";
}

int main(int argc, char* argv[])
{

  srand(time(NULL));
  int nb_spots = 4;
  int nb_colors = 5;
  int perfect = 0;
  int colors_only = 0;

  vector<string> colors_names = {"Red", "Orange", "Yellow",
  "Green", "Blue", "Indigo", "Violet", "Gray", "Black", "Pink"};

  Solver master_node(nb_spots, nb_colors);
  vector<Solver*> solvers;
  for (int i=0; i < nb_colors; i++)
    solvers.push_back(new Solver(i, nb_spots, nb_colors));

  int rank, nb_instances;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_instances);

  bool found = 0;
  for (auto const &s: solvers)
  {
    while (!(s->possible_guesses.empty()))
    {
      vector<int> guess = s->give_next_guess();
      found = master_node.check_solution(master_node.secret, guess, &perfect, &colors_only);
      if (found)
        break;
      s->update_possible_guesses(guess, perfect, colors_only);
      //cout << "\n";
      //s->print_possible_guesses();
      print_colors(guess, nb_spots, colors_names, perfect, colors_only);
    }
    if (found)
    {
      cout << "Solution found by solver " << s->id << "\n";
      break;
    }

  }


  cout << "\nSolution: ";
  print_colors(master_node.secret, nb_spots, colors_names);

  MPI_Finalize();
  return 0;
}
