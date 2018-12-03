#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

int main()
{
  srand(time(NULL));

  int nb_spots = 4;
  int nb_colors = 10;

  string colors[nb_colors] = {"Red", "Orange", "Yellow", "Green", "Blue",
                             "Indigo", "Violet", "Gray", "Black", "Pink"};
  string secret[nb_spots];
  string new_color;
  int i = 0;

  while(i < nb_spots)
  {
     new_color = colors[rand() % 3];
     secret[i] = new_color;
     i++;
  }

  for (auto x: secret) { cout << x << "\n"; }

  return 0;
}
