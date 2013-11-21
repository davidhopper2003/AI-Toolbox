#include <fstream>
#include <iostream>
#include <string>

#include <MDPToolbox/MDP.hpp>
#include <MDPToolbox/Policy.hpp>
#include <MDPToolbox/Experience.hpp>

#include "boost/filesystem.hpp"

using std::cout;
using std::cerr;

int main(int argc, char * argv[]) {
    size_t S = 96, A = 2;

    if ( argc < 2 || argc > 3 ) {
        cerr << "Usage: solve_mdp filename [debug]\n";
        return -1;
    }
    MDPToolbox::Experience t(S, A);

    bool debug = false;
    if ( argc == 3 ) {
        boost::system::error_code returnedError;
        boost::filesystem::create_directories( "debug", returnedError );
        if ( returnedError ) {
            cerr << "ERR -- Could not create directory 'debug', debug files will not be created.\n";
        }
        else {
            debug = true;
        }
    }

    // LOADING TABLE
    cout << "Loading Table.\n\n";
    {
        std::ifstream tableFile(argv[1]);
        if ( ! ( tableFile >> t ) ) {
            cerr << "ERR -- Could not load specified table.\n";
            return 1;
        }
    }
    cout << "Table loaded correctly.\n\n";

    // OUTPUT LOADED TABLE
    if ( debug ) {
        cout << "DBG -- Outputting table for sanity check...\n";

        std::ofstream tableFile("debug/table_sanity.txt");
        tableFile << t;

        cout << "DBG -- Done.\n\n";
    }

    // NORMALIZING DATA
    auto mdpdata = t.getMDP();
    cout << "MDP extracted.\n\n";

    if ( debug ) {
        cout << "DBG -- Saving MDP to file...\n";
        {
            std::ofstream outfile("debug/transitionprobabilities_sanity.txt");
            outfile.precision(4);
            outfile << std::fixed;
            int counter = 1;
            for ( size_t a = 0; a < A; a++ ) {
                for ( size_t i = 0; i < S; i++ ) {
                    for ( size_t j = 0; j < S; j++ ) {
                        if ( ! ( counter % 21) ) { outfile << "\t\t\t"; counter = 1; }
                        outfile << std::get<0>(mdpdata)[i][j][a] << "\t";
                        counter ++;
                    }
                    outfile << "\n";
                    counter = 1;
                }
                outfile << "\n\n\n\n\n";
                counter = 1;
            }
        }
        {
            std::ofstream outfile("debug/rewardsnormalized_sanity.txt");
            outfile.precision(4);
            outfile << std::fixed;
            int counter = 1;
            for ( size_t a = 0; a < A; a++ ) {
                for ( size_t i = 0; i < S; i++ ) {
                    for ( size_t j = 0; j < S; j++ ) {
                        if ( ! ( counter % 21) ) { outfile << "\t\t\t"; counter = 1; }
                        outfile << std::get<1>(mdpdata)[i][j][a] << "\t";
                        counter ++;
                    }
                    outfile << "\n";
                    counter = 1;
                }
                outfile << "\n\n\n\n\n";
                counter = 1;
            }
        }
        cout << "DBG -- MDP saved.\n\n";
    }

    // LOADING TABLE
    cout << "Loading table in MDPToolbox...\n";

    MDPToolbox::MDP mdp(S, A);

    mdp.setMDP(mdpdata);
    cout << "Table loaded.\n\n";

    // SOLVING MDP
    cout << "Solving MDP...\n";
    bool done = mdp.valueIteration();
    cout << "MDP Solved.\n";
    cout << "+--> Did we actually solve the MDP? " << ( done ? "YES": "NO" ) << "\n\n";

    // CREATING POLICY
    cout << "Creating Policy...\n";
    {
        std::ofstream outfile("policy.txt");
        outfile << mdp.getPolicy();
        outfile.close();
    }
    cout << "Policy created.\n\n";

    // Checking policy with Qtable:
    for (size_t s = 0; s < S; s++)
        cout << s << " " << mdp.getGreedyAction(s) << "\n";
    return 0;
}