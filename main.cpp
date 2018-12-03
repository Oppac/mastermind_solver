#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>

using namespace std;

int main()
{
  srand(time(NULL));

  int nb_spots = 4;
  int nb_colors = 10;

  string colors[nb_colors] = {"Red", "Orange", "Yellow",
   "Green", "Blue", "Indigo", "Violet", "Gray", "Black", "Pink"};
  vector<string> secret;
  string new_color;
  int i = 0;
  bool in_secret;

  while(i < nb_spots)
  {
     new_color = colors[rand() % 10];
     in_secret = std::find(secret.begin(), secret.end(), new_color) != secret.end();
     if (!in_secret){
      secret.push_back(new_color);
      i++;
     }
  }

  for (auto x: secret) { cout << x << "\n"; }

  return 0;
}
