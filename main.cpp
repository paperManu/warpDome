#include "main.h"

#include <math.h>

using namespace std;

/*************/
void printHelp()
{
    cout << "WarpMesh, a tool to create fulldome dedicated warping mesh, for Blender" << endl;
    cout << "Parameters:" << endl;
    cout << "    -r : set the square resolution of the mesh (default: 5)" << endl;
    cout << "    -f : set the target field of view, in degrees (default: 180.0)" << endl;
    cout << "    -z : set the shift ratio between lens and head positions (default: 0.4)" << endl;
    cout << "    -a : set the aspect ratio (default: 1.0)" << endl;
    cout << "    -s : do not display summary of parameters, only final result" << endl;
    cout << "    -h : display this help" << endl;
}

/*************/
int main(int argc, char** argv)
{
    auto silent = false; // If false, only output the warp mesh file
    auto resolution = Point<int>(5, 5);
    auto outputFov = M_PI;
    auto zShiftRatio = 0.4;
    auto aspect = 1.0;

    for (int i = 1; i < argc;)
    {
        if (string(argv[i]) == "-r" && i < argc - 1)
        {
            auto res = (stoi(string(argv[i + 1])) / 2) * 2 + 1;
            resolution = Point<int>(res, res);
            i += 2;
        }
        else if (string(argv[i]) == "-f" && i < argc - 1)
        {
            auto fov = stof(argv[i + 1]);
            outputFov = fov * M_PI / 180.0;
            i += 2;
        }
        else if (string(argv[i]) == "-z" && i < argc - 1)
        {
            auto z = stof(argv[i + 1]);
            zShiftRatio = z;
            i += 2;
        }
        else if (string(argv[i]) == "-a" && i < argc - 1)
        {
            auto a = stof(argv[i + 1]);
            aspect = a;
            i += 2;
        }
        else if (string(argv[i]) == "-s")
        {
            silent = true;
            ++i;
        }
        else if (string(argv[i]) == "-h")
        {
            printHelp();
            exit(0);
        }
        else
            ++i;
    }

    // Compute the input fov
    auto inputFov = M_PI + 2.0 * atan(zShiftRatio);

    // Print the parameters
    if (!silent)
    {
        cout << "Mesh square resolution: " << resolution.x << " x " << resolution.y << endl;
        cout << "Target field of view: " << outputFov << endl;
        cout << "Equivalent field of view: " << inputFov * 180.0 / M_PI << endl;
        cout << "Shift ratio: " << zShiftRatio << endl;
        cout << "Aspect ratio: " << aspect << endl;
        cout << endl;
    }

    // Create the mesh
    vector<vector<Point<double>>> warpMesh;
    warpMesh.resize(resolution.y);
    for (auto& warpLine : warpMesh)
        warpLine.resize(resolution.x);

    // Compute the UV for all mesh elements
    auto localRadius = cos(asin(zShiftRatio));
    for (int y = 0; y < resolution.y; ++y)
    {
        for (int x = 0; x < resolution.x; ++x)
        {
            auto normalized = Point<double>((double)(x - (resolution.x - 1) / 2) * 2.0 / (double)(resolution.x - 1),
                                           (double)(y - (resolution.y - 1) / 2) * 2.0 / (double)(resolution.y - 1));
            auto angle = sqrt(pow(normalized.x, 2.0) + pow(normalized.y, 2.0)) * outputFov / 2.0;
            auto hyp = Point<double>(sin(angle), cos(angle) - zShiftRatio);
            auto alpha = M_PI / 2.0 - asin(1.0 / sqrt(hyp.x*hyp.x + hyp.y*hyp.y) * (sin(M_PI / 2.0 - angle) - zShiftRatio));

            auto normalizedAlpha = alpha / inputFov * 2.0;
            auto length = sqrt(normalized.x*normalized.x + normalized.y*normalized.y);

            //cout << normalized.x << " " << normalized.y
            //     << " -> " << angle
            //     << " -> " << hyp.x << " " << hyp.y
            //     << " -> " << alpha << " " << normalizedAlpha
            //     << " -> " << length << endl;

            if (length == 0.0)
                length = 1.0;
            warpMesh[y][x].x = normalizedAlpha * normalized.x / length / 2.0 + 0.5;
            warpMesh[y][x].y = normalizedAlpha * normalized.y / length / 2.0 + 0.5;
        }
    }

    // Print the result
    cout << 2 << endl;
    cout << resolution.x << " " << resolution.y << endl;

    for (int y = 0; y < resolution.y; ++y)
        for (int x = 0; x < resolution.x; ++x)
        {
            auto normalized = Point<double>((double)(x - (resolution.x - 1) / 2) * 2.0 / (double)(resolution.x - 1) * aspect,
                                           (double)(y - (resolution.y - 1) / 2) * 2.0 / (double)(resolution.y - 1));
            auto& point = warpMesh[y][x];

            cout << normalized.x << " " << normalized.y << " " << point.x << " " << point.y << " " << 1.0 << endl;
        }
}
