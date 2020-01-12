#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main()
{
    vector<float> v{};
    string s;

    // keep asking user to enter a number until he enters "p"
    while (true)
    {
        cin >> s;

        // if special character was entered leave the loop
        if (s == "p")
            break;

        // check user input for validity
        try
        {
            v.push_back(stof(s));
        }
        catch (...)
        {
            cout << "enter valid floating point number" << endl;
        }
    }

    // don't do anything if a user didn't enter a number
    if (v.empty())
        return 0;

    cout << '\n';
    cout << "initial values" << '\n';

    // print entered values
    for (float f : v)
        cout << f << ' ';

    // do task - divide entry by it's index if the value is less than the index
    for (size_t i{ 2 }; i < v.size(); i += 3)
    {
        float& f{ v[i] };
        if (f < i)
            f /= i;
    }

    cout << '\n';
    cout << "values after operation" << '\n';

    // print modified values
    for (float f : v)
        cout << f << ' ';

    // open binary file
    FILE* fp{ std::fopen("file.binary", "wb") };
    if (!fp)
    {
        cout << "file opening failed" << endl;
        return 1;
    }

    // write modified data
    if(fwrite(v.data(), sizeof(float), v.size(), fp) != v.size())
        cout << "file writing failed" << endl;

    // close the file
    if(fclose(fp) != 0)
        cout << "file closing failed" << endl;

	return 0;
}
