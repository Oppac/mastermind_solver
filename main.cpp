#include "mpi.h"

#include <iostream>
#include <ctime>

using namespace std;

void reinitialize_arr(int* arr, int arr_size);
bool in_array(int* arr, int arr_size, int nb);
void new_secret(int* solution, int nb_spots, int nb_colors);
bool check_solution(int* secret, int* guess, int *perfect, int *colors_only, int nb_spots);
void print_colors(int* secret, int nb_spots, string* colors_names);
void print_colors(int* secret, int nb_spots, string* colors_names, int perfect, int colors_only);

//Empty the array of its previous colors
void reinitialize_arr(int* arr, int arr_size)
{
  for (int i = 0; i < arr_size; ++i)
    arr[i] = 0;
}

//Check if the color is already in the array
bool in_array(int* arr, int arr_size, int nb)
{
  for (int i = 0; i < arr_size; ++i)
  {
    if (arr[i] == nb)
      return 1;
  }
  return 0;
}

//Give a new random system
void new_secret(int* secret, int nb_spots, int nb_colors)
{
  int i = 0, new_color;
  reinitialize_arr(secret, nb_spots);
  while(i < nb_spots)
  {
    new_color = 1 + rand() % nb_colors;
    if (!in_array(secret, nb_spots, new_color))
    {
      secret[i] = new_color;
      i++;
    }
  }
}

//Check if the guess is correct or not
bool check_solution(int* secret, int* guess, int *perfect, int *colors_only, int nb_spots)
{
  int b = 1;
  *perfect = 0;
  *colors_only = 0;
  for (int i = 0; i < nb_spots; ++i)
  {
    if (secret[i] == guess[i])
      (*perfect)++;
    else
    {
      if (in_array(secret, nb_spots, guess[i]))
        (*colors_only)++;
      b = 0;
    }
  }
  return b;
}

//Print the colors
void print_colors(int* secret, int nb_spots, string* colors_names)
{
  for (int i = 0; i < nb_spots; ++i)
    cout << colors_names[secret[i]] << " ";
  cout << "\n";
}

//Print the colors and the perfect and color only values
void print_colors(int* secret, int nb_spots, string* colors_names, int perfect, int colors_only)
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

  string colors_names[] = {"None", "Red", "Orange", "Yellow",
  "Green", "Blue", "Indigo", "Violet", "Gray", "Black", "Pink"};

  int secret[nb_spots];
  new_secret(secret, nb_spots, nb_colors);

  int perfect = 0;
  int perfect_values[nb_spots];
  reinitialize_arr(perfect_values, nb_spots);

  int colors_only = 0;
  int colors_only_values[nb_spots];
  reinitialize_arr(colors_only_values, nb_spots);

  int rank, nb_instances;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_instances);

  //cout << rank << "\n";
  //cout << nb_instances << "\n";

  bool found = 0;
  do {
      MPI_Status status;
      int guess[nb_spots];
      new_secret(guess, nb_spots, nb_colors);

      found = check_solution(secret, guess, &perfect, &colors_only, nb_spots);
      print_colors(guess, nb_spots, colors_names, perfect, colors_only);
  } while (!found);

  cout << "\nSolution: ";
  print_colors(secret, nb_spots, colors_names);

  MPI_Finalize();
  return 0;
}
