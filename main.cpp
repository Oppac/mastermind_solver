#include <iostream>
#include <ctime>

using namespace std;

void print_colors(int* secret, int nb_spots, string* colors_names);
void new_solution(int* solution, int nb_spots, int nb_colors);
bool check_solution(int* secret, int* guess, int nb_spots);

bool in_array(int* arr, int arr_size, int nb);
void reinitialize_arr(int* arr, int arr_size);

int main()
{
  srand(time(NULL));
  int nb_spots = 4;
  int nb_colors = 4;

  string colors_names[] = {"Red", "Orange", "Yellow",
  "Green", "Blue", "Indigo", "Violet", "Gray", "Black", "Pink"};

  int secret[nb_spots];
  new_solution(secret, nb_spots, nb_colors);
  cout << "\n";

  int guess[nb_spots];
  do {
    new_solution(guess, nb_spots, nb_colors);
    print_colors(guess, nb_spots, colors_names);
  } while(check_solution(secret, guess, nb_spots) == 0);

  cout << "\nSecret: " << "\n";
  print_colors(secret, nb_spots, colors_names);
  cout << "Solution: " << "\n";
  print_colors(guess, nb_spots, colors_names);


  return 0;
}


void new_solution(int* solution, int nb_spots, int nb_colors)
{
  int i = 0, new_color;
  reinitialize_arr(solution, nb_spots);
  while(i < nb_spots)
  {
    new_color = rand() % nb_colors;
    if (in_array(solution, nb_spots, new_color) == 0)
    {
      solution[i] = new_color;
      i++;
    }
  }
}

bool check_solution(int* secret, int* guess, int nb_spots)
{
  for (int i = 0; i < nb_spots; ++i)
  {
    if (secret[i] != guess[i])
    {
      return 0;
    }
  }
  return 1;
}

void print_colors(int* secret, int nb_spots, string* colors_names)
{
  for (int i = 0; i < nb_spots; ++i)
  {
    cout << " " << colors_names[secret[i]] << " ";
  }
  cout << "\n";
}

bool in_array(int* arr, int arr_size, int nb)
{
  for (int i = 0; i < arr_size; ++i)
  {
    if (arr[i] == nb)
    {
      return 1;
    }
  }
  return 0;
}

void reinitialize_arr(int* arr, int arr_size)
{
  for (int i = 0; i < arr_size; ++i)
  {
    arr[i] = -1;
  }
}
