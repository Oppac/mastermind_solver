#include <iostream>
#include <ctime>

using namespace std;

void print_colors(int* secret, int nb_spots, string* colors_names);
void new_secret(int* secret, int nb_spots, int nb_colors);
bool in_array(int* arr, int arr_size, int nb);

int main()
{
  int nb_spots = 4;
  int nb_colors = 10;

  string colors_names[nb_colors] = {"Red", "Orange", "Yellow",
  "Green", "Blue", "Indigo", "Violet", "Gray", "Black", "Pink"};

  int secret[4];
  new_secret(secret, nb_spots,nb_colors);
  print_colors(secret, nb_spots, colors_names);

return 0;
}

void new_secret(int* secret, int nb_spots, int nb_colors)
{
  srand(time(NULL));
  int i = 0, new_color;
  while(i < nb_spots)
  {
    new_color = rand() % nb_colors;
    if (!(in_array(secret, nb_spots, new_color)))
    {
      secret[i] = new_color;
      i++;
    }
  }
}

bool in_array(int* arr, int arr_size, int nb)
{
  for (int i = 0; i < arr_size; i++)
  {
    if (arr[i] == nb)
    {
      return 1;
    }
  }
  return 0;
}

void print_colors(int* secret, int nb_spots, string* colors_names)
{
  for (int i = 0; i < nb_spots; ++i)
  {
    cout << colors_names[secret[i]] << "\n";
  }
}
