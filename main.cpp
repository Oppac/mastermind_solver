#include <algorithm>
#include <ctime>
#include <iostream>
#include <numeric>
#include <vector>

#include "mpi.h"

//Flags to send between the master node and the solvers.
const int NEW_GUESS = -1;
const int EVAL_GUESS = -2;
const int FOUND = -3;
const int NO_MORE_GUESSES = -4;

class Solver
{
public:
  int id;
  int nb_spots;
  int nb_colors;

  std::vector<int> secret;
  std::vector<std::vector<int>> possible_guesses;
  std::vector<std::vector<int>> all_guess;

  Solver(int id, int nb_spots, int nb_colors, std::vector<std::vector<int>> all_guess)
  {
    this->id = id;
    this->nb_spots = nb_spots;
    this->nb_colors = nb_colors;
    this->all_guess = all_guess;
    this->possible_guesses = solver_possible_guess();
  }

  //Special master constructor
  Solver(int nb_spots, int nb_colors)
  {
    this->nb_spots = nb_spots;
    this->nb_colors = nb_colors;
    this->secret = new_secret(nb_spots, nb_colors);
  }

  //Utility method -> print the remaining possible guesses.
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

  //Utility function -> compute the factorial of n.
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

  //Return the next guess in the possible guesses list.
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

  //Remove the unplausible guesses using the feedback send by the master node.
  void update_possible_guesses(std::vector<int> old_guess, int perfect, int colors_only)
  {
    int tmp_perfect = perfect;
    int tmp_colors_only = colors_only;
    std::vector<int> new_guess;
    if (perfect == 0)
      possible_guesses.clear();
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


  //Check two system against each other and give their perfect and color-only score.
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

  //Give a new random secret.
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

void run_master_node(Solver& master, int nb_solvers, int nb_spots, bool &found, vector<string> colors_names);
void run_solver_node(Solver& s, int nb_spots, bool &found);

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
}

//Master node
void run_master_node(Solver& master, int nb_solvers, int nb_spots, bool &found, vector<string> colors_names)
{
  int msg;
  vector<int> guess;

  for (int node=1; node < nb_solvers; node++)
  {
    MPI_Send(&NEW_GUESS, 1, MPI_INT, node, 0, MPI_COMM_WORLD);
    MPI_Recv(&msg, 1, MPI_INT, node, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
    if(msg == EVAL_GUESS)
    {
      guess.resize(nb_spots);
      MPI_Recv(&guess[0], nb_spots, MPI_INT, node, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
      if (guess[0] == NO_MORE_GUESSES)
        break;
      else
      { //Evaluate the received guess.
        int perfect = 0;
        int colors_only = 0;
        found = master.check_solution(master.secret, guess, &perfect, &colors_only);
        cout << "From solver " << node << ": ";
        print_colors(guess, nb_spots, colors_names, perfect, colors_only);
        cout << "\n";
        MPI_Send(&perfect, 1, MPI_INT, node, 0, MPI_COMM_WORLD);
        MPI_Send(&colors_only, 1, MPI_INT, node, 0, MPI_COMM_WORLD);
        if (found)
        { //Other solvers need to be stopped if the secret has been found.
          cout << "Solution have been found by solver " << node << "\n";
          cout << "Solution: ";
          print_colors(master.secret, nb_spots, colors_names);
          for (int node=1; node < nb_solvers; node++)
            MPI_Send(&FOUND, 1, MPI_INT, node, 0, MPI_COMM_WORLD);
          break;
        }
      }
    }
  }
}

//Solver node
void run_solver_node(Solver& s, int nb_spots, bool &found)
{
  int msg;
  MPI_Recv(&msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

  if (msg == NEW_GUESS)
  {
    vector<int> guess = s.give_next_guess();
    if (!(guess.empty()))
    { //Send a guess to be evaluated then act on the feedback.
      int perfect, colors_only;
      MPI_Send(&EVAL_GUESS, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      MPI_Send(&guess[0], nb_spots, MPI_INT, 0, 0, MPI_COMM_WORLD);
      MPI_Recv(&perfect, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
      MPI_Recv(&colors_only, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
      s.update_possible_guesses(guess, perfect, colors_only);
    }
    else
      MPI_Send(&NO_MORE_GUESSES, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }
  else if (msg == FOUND)
    found = 1;
}


int main(int argc, char* argv[])
{

  srand(time(NULL));
  int nb_spots = 4;
  int nb_colors = 10;

  vector<string> colors_names = {"Red", "Orange", "Yellow",
  "Green", "Blue", "Indigo", "Violet", "Gray", "Black", "Pink"};

  Solver master_node(nb_spots, nb_colors);
  std::vector<std::vector<int>> all_guess = master_node.all_possible_guess();

  vector<Solver*> solvers;
  for (int i=0; i < nb_colors; i++)
    solvers.push_back(new Solver(i, nb_spots, nb_colors, all_guess));

  int rank, nb_solvers;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_solvers);


  if (!(nb_solvers == nb_colors+1))
  { //Because each solver node is responsible for exactly one color
    cout << "The number of nodes need to be exactly the number of colors + 1";
    exit(EXIT_FAILURE);
  }


  //Loop until the solution is found
  bool found = 0;
  do
  {
    if (rank == 0)
    {
      run_master_node(master_node, nb_solvers, nb_spots, found, colors_names);
    }
    else
    {
      for (auto const &s: solvers)
      {
        if (rank-1 == s->id)
          run_solver_node(*s, nb_spots, found);
      }
    }
  } while(!found);


  //Non-parallel solver
  /*
  bool found = 0;
  int perfect = 0, colors_only = 0;
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
  */

  MPI_Finalize();
  return 0;
}
