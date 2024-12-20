#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <ctime>
#include <sys/stat.h>
#include <cstdlib>

#define DELIM "\n"

using namespace std;

enum class Color
{
    RED, ORANGE, YELLOW, GREEN, BLUE, INDIGO, VIOLET
};

const vector<string> COLOR_NAMES = {"red", "orange", "yellow", "green", "blue", "indigo", "violet"};

class Object
{
public:
    virtual ~Object() = default;
    virtual double calculate_area() const = 0;
    virtual string get_type() const = 0;
    virtual void read_parameters(ifstream &file) = 0;
    virtual void write_parameters(ostream &out) const = 0;

    Color color;
    double area;
};

class Circle : public Object
{
public:
    int x, y, radius;

    double calculate_area() const override
    {
        return M_PI * radius * radius;
    }
    
    string get_type() const override
    {
        return "Circle";
    }

    void read_parameters(ifstream &file) override
    {
        file >> x >> y >> radius;
    }

    void write_parameters(ostream &out) const override
    {
        out << x << ", " << y << ", " << radius;
    }
};

class Rectangle : public Object
{
public:
    int x1, y1, x2, y2;

    double calculate_area() const override
    {
        return abs(x2 - x1) * abs(y2 - y1);
    }

    string get_type() const override
    {
        return "Rectangle";
    }

    void read_parameters(ifstream &file) override
    {
        file >> x1 >> y1 >> x2 >> y2;
    }

    void write_parameters(ostream &out) const override
    {
        out << x1 << ", " << y1 << ", " << x2 << ", " << y2;
    }
};

class Triangle : public Object
{
public:
    int x1, y1, x2, y2, x3, y3;

    double calculate_area() const override
    {
        return fabs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0);
    }

    string get_type() const override
    {
        return "Triangle";
    }

    void read_parameters(ifstream &file) override
    {
        file >> x1 >> y1 >> x2 >> y2 >> x3 >> y3;
    }

    void write_parameters(ostream &out) const override
    {
        out << x1 << ", " << y1 << ", " << x2 << ", " << y2 << ", " << x3 << ", " << y3;
    }
};

class Program
{
public:
    static void run()
    {
        srand(static_cast<unsigned>(time(nullptr)));
        int is_sorted = 0;
        int count = get_int("Enter the number of shapes to generate: ", 1, INT_MAX);
        generate_file("input.txt", count);

        vector<Object*> objects = read_objects("input.txt");
        
        int direction = get_int("Choose sorting order (1 for ascending, 0 for descending): ", 0, 1);
        //copy_file("input.txt", "output.txt");
        write_objects("output.txt", objects, is_sorted);
        bubble_sort(objects, direction);
        is_sorted = 1;
        //copy_file("input.txt", "output.txt");
        write_objects("output.txt", objects, is_sorted);
        write_statistics("stat.txt", objects);

        for (size_t i = 0; i < objects.size(); ++i)
        {
            delete objects[i];
        }
    }

private:
    static int get_int(const string &prompt, int min, int max)
    {
        int number;
        while (true)
        {
            cout << prompt;
            cin >> number;
            if (cin.fail() || number < min || number > max)
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Try again.\n";
            }
            else
            {
                break;
            }
        }
        return number;
    }

    static void copy_file (const string &input, const string &output)
    {

        ifstream ini_file(input);
        ofstream out_file(output);
        string line;
        
            if (ini_file && out_file)
            {
                //getline(ini_file, line);
                while (getline(ini_file, line))
                {
                    out_file << line << "\n";
                }
                cout << "Copy Finished \n";
            }
            else
            {
               
                printf("Cannot read File");
            }
        
            ini_file.close();
            out_file.close();
    }
    
    static void generate_file(const string &filename, int count)
    {
        ofstream file(filename);
        if (!file) {
            throw runtime_error("Failed to open file for generation");
        }

        for (int i = 0; i < count; ++i)
        {
            int shapeType = rand() % 3;
            const string &color = COLOR_NAMES[rand() % COLOR_NAMES.size()];

            if (shapeType == 0)
            {
                int x = rand() % 100, y = rand() % 100, radius = rand() % 50 + 1;
                file << "Circle " << x << " " << y << " " << radius << " " << color << "\n";
            }
            else if (shapeType == 1)
            {
                int x1 = rand() % 100, y1 = rand() % 100, x2 = x1 + rand() % 50 + 1, y2 = y1 + rand() % 50 + 1;
                file << "Rectangle " << x1 << " " << y1 << " " << x2 << " " << y2 << " " << color << "\n";
            }
            else
            {
                int x1 = rand() % 100, y1 = rand() % 100, x2 = rand() % 100, y2 = rand() % 100, x3 = rand() % 100, y3 = rand() % 100;
                file << "Triangle " << x1 << " " << y1 << " " << x2 << " " << y2 << " " << x3 << " " << y3 << " " << color << "\n";
            }
        }
    }

    static vector<Object*> read_objects(const string &filename)
    {
        ifstream file(filename);
        if (!file)
        {
            throw runtime_error("Failed to open input file");
        }
        vector<Object*> objects;
        string type;
        while (file >> type) {
            Object* object = nullptr;
            if (type == "Circle")
            {
                object = new Circle();
            }
            else if (type == "Rectangle")
            {
                object = new Rectangle();
            }
            else if (type == "Triangle")
            {
                object = new Triangle();
            }

            if (object)
            {
                object->read_parameters(file);
                string colorName;
                file >> colorName;

                size_t colorIndex = 0;
                for (size_t i = 0; i < COLOR_NAMES.size(); ++i)
                {
                    if (COLOR_NAMES[i] == colorName)
                    {
                        colorIndex = i;
                        break;
                    }
                }
                object->color = static_cast<Color>(colorIndex);
                object->area = object->calculate_area();
                objects.push_back(object);
            }
        }
        return objects;
    }

    static void bubble_sort(vector<Object*> &objects, int direction)
    {
        for (size_t i = 0; i < objects.size() - 1; ++i)
        {
            for (size_t j = 0; j < objects.size() - i - 1; ++j)
            {
                bool flag = direction ? (objects[j]->area > objects[j + 1]->area) : (objects[j]->area < objects[j + 1]->area);
                if (flag)
                {
                    Object* temp = objects[j];
                    objects[j] = objects[j + 1];
                    objects[j + 1] = temp;
                }
            }
        }
    }

    //static void writeObjects(const string &filename, const vector<Object*> &objects) {
    static void write_objects(const string &filename, const vector<Object*> &objects, int flag)
    {
        ofstream file(filename);
        if (!file)
        {
            throw runtime_error("Failed to open output file");
        }
        
        file << "Number of objects: " << objects.size() << "\n\n";
        file << "Initial data:\n";
        
        //if (flag == 0)
        //{
        for (size_t i = 0; i < objects.size(); ++i)
        {
            file << objects[i]->get_type() << " ";
            objects[i]->write_parameters(file);
            file << " " << COLOR_NAMES[static_cast<int>(objects[i]->color)] << "\n";
        }
        
        /*
        for (int i = 0; i < objects.size(); i++)
        {
            if (objects[i]->getType() == "Circle")
            {
                objects[i]->writeParameters(file);
                file << " " << COLOR_NAMES[static_cast<int>(objects[i]->color)] << "\n";
            }
            else if (objects[i]->getType() == "Rectangle")
            {
                objects[i]->writeParameters(file);
                file << " " << COLOR_NAMES[static_cast<int>(objects[i]->color)] << "\n";
            }
            else if (objects[i]->getType() == "Triangle")
            {
                objects[i]->writeParameters(file);
                file << " " << COLOR_NAMES[static_cast<int>(objects[i]->color)] << "\n";
            }
        }
         */
        //}
        if (flag == 1)
        {
            file << "\nSorted data:\n";
            for (size_t i = 0; i < objects.size(); ++i)
            {
                file << objects[i]->get_type() << " ";
                objects[i]->write_parameters(file);
                file << " " << COLOR_NAMES[static_cast<int>(objects[i]->color)] << " Area: " << objects[i]->area << "\n";
            }
        }
    }
    
    
    static void write_statistics(const string &filename, const vector<Object*> &objects) {
        ofstream file(filename);
        if (!file)
        {
            throw runtime_error("Failed to open statistics file");
        }

        file << "Execution time: simulated\n";
        file << "Number of objects processed: " << objects.size() << "\n";
    }
};

int main()
{
    Program::run();
    return 0;
}
