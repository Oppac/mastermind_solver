#include <iostream>
#include <ctime>

using namespace std;

void print_colors(int* secret, int nb_spots, string* colors_names);
void new_secret(int* solution, int nb_spots, int nb_colors);
bool check_solution(int* secret, int* guess, int *perfect, int *colors_only, int nb_spots);

bool in_array(int* arr, int arr_size, int nb);
void reinitialize_arr(int* arr, int arr_size);

int main()
{
  srand(time(NULL));
  int nb_spots = 4;
  int nb_colors = 5;

  string colors_names[] = {"None", "Red", "Orange", "Yellow",
  "Green", "Blue", "Indigo", "Violet", "Gray", "Black", "Pink"};

  int secret[nb_spots];
  new_secret(secret, nb_spots, nb_colors);
  int perfect = 0;
  int colors_only = 0;

  int guess[nb_spots];
  do {
    cout << "Perfect: " << perfect << " - Color Only: " << colors_only << "\n";
    cout << "\n";
    new_secret(guess, nb_spots, nb_colors);
    print_colors(guess, nb_spots, colors_names);
  } while(check_solution(secret, guess, &perfect, &colors_only, nb_spots) == 0);

  cout << "\nSecret: " << "\n";
  print_colors(secret, nb_spots, colors_names);
  cout << "Solution: " << "\n";
  print_colors(guess, nb_spots, colors_names);

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
    if (in_array(secret, nb_spots, new_color) == 0)
    {
      secret[i] = new_color;
      i++;
    }
  }
}

//Check if the secret and the guess match
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

//Print the array, replacing the numbers by their respective name
void print_colors(int* secret, int nb_spots, string* colors_names)
{
  for (int i = 0; i < nb_spots; ++i)
    cout << " " << colors_names[secret[i]] << " ";
  cout << "\n";
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

//Empty the array of its previous colors
void reinitialize_arr(int* arr, int arr_size)
{
  for (int i = 0; i < arr_size; ++i)
    arr[i] = 0;
}
