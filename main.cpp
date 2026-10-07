// Compilation : g++ -O2 -std=c++17 recherche_10M_corrige.cpp -o remplacer
// Version corrigee : nouveau nom de fichier de test (evite un ancien fichier
// sans "xromanx"), messages de progression, verification du chargement,
// et pause a la fin pour que la console ne se ferme pas.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <random>

using namespace std;
using namespace std::chrono;

const size_t NB_MOTS = 10000000;
const string FICHIER = "mots_10M_test.txt";   // nouveau nom

// Position de la sous-chaine continue (ou string::npos)
size_t rechercher(const string& mot, const string& cherche)
{
    return mot.find(cherche);
}

// Remplace toutes les occurrences, renvoie leur nombre
size_t remplacer(string& mot, const string& cherche, const string& remplace)
{
    size_t nb = 0;
    for (size_t pos = rechercher(mot, cherche); pos != string::npos;
         pos = mot.find(cherche, pos + remplace.size()))
    {
        mot.replace(pos, cherche.size(), remplace);
        nb++;
    }
    return nb;
}

// Cree le fichier s'il n'existe pas (un "xromanx" tous les 1 000 000 de mots)
void creer_fichier()
{
    if (ifstream(FICHIER)) return;
    cout << "Creation de " << FICHIER << " (quelques secondes)..." << endl;
    mt19937 gen(42);
    ofstream f(FICHIER);
    for (size_t i = 0; i < NB_MOTS; i++)
    {
        if (i % 1000000 == 0) { f << "xromanx\n"; continue; }
        for (int k = 4 + gen() % 9; k > 0; k--) f << (char)('a' + gen() % 26);
        f << '\n';
    }
}

// Memoire des donnees (octets)
size_t memoire(const vector<string>& v)
{
    size_t total = sizeof(v) + v.capacity() * sizeof(string);
    for (const string& s : v)
        if (s.capacity() > 15) total += s.capacity() + 1;
    return total;
}

auto ms = [](auto a, auto b) { return duration<double, milli>(b - a).count(); };

// Attend Entree avant de fermer la console
void pause()
{
    cout << "\nAppuie sur Entree pour quitter...";
    cin.ignore();
    cin.get();
}

int main()
{
    creer_fichier();

    string cherche, remplace;
    cout << "Fichier utilise : " << FICHIER << endl;
    cout << "Sous-chaine a chercher : ";
    cin >> cherche;
    cout << "Sous-chaine de remplacement : ";
    cin >> remplace;

    // 1) Chargement
    cout << "\nChargement du fichier..." << endl;
    vector<string> mots;
    mots.reserve(NB_MOTS);
    auto t0 = steady_clock::now();
    ifstream f(FICHIER);
    if (!f) { cout << "Erreur : impossible d'ouvrir " << FICHIER << endl; pause(); return 1; }
    for (string m; f >> m;) mots.push_back(std::move(m));
    auto t1 = steady_clock::now();
    if (mots.empty()) { cout << "Erreur : le fichier est vide, supprime-le et relance." << endl; pause(); return 1; }
    size_t octets = memoire(mots);
    cout << mots.size() << " mots charges. Recherche en cours..." << endl;

    // 2) Recherche (+ premier mot trouve + plus grand mot qui contient la sous-chaine)
    size_t trouves = 0, indexPremier = 0, indexMax = 0;
    string premier = "aucun", apres = "aucun", plusGrand = "aucun";
    auto t2 = steady_clock::now();
    for (size_t i = 0; i < mots.size(); i++)
        if (rechercher(mots[i], cherche) != string::npos)
        {
            if (trouves == 0) { premier = mots[i]; indexPremier = i; indexMax = i; }
            else if (mots[i].size() > mots[indexMax].size()) indexMax = i;
            trouves++;
        }
    if (trouves > 0) plusGrand = mots[indexMax];
    auto t3 = steady_clock::now();

    // 3) Remplacement dans le vector
    size_t remplacements = 0;
    auto t4 = steady_clock::now();
    if (trouves > 0)
    {
        for (string& m : mots) remplacements += remplacer(m, cherche, remplace);
        apres = mots[indexPremier];
    }
    auto t5 = steady_clock::now();

    // Resultats
    cout << "\n================ RESULTATS ================" << endl;
    cout << "Fichier lu          : " << FICHIER << " (" << mots.size() << " mots, non modifie)" << endl;
    cout << "Sous-chaine         : \"" << cherche << "\" -> \"" << remplace << "\"" << endl;
    cout << "Mots trouves        : " << trouves << endl;
    cout << "Remplacements       : " << remplacements << " (dans le vector)" << endl;
    if (trouves > 0)
        cout << "Premier mot trouve  : " << premier << "  (position " << indexPremier + 1 << ")" << endl
             << "Dans le vector      : " << apres << endl
             << "Plus grand mot      : " << plusGrand << "  (" << plusGrand.size()
             << " lettres, position " << indexMax + 1 << ")" << endl;
    else
        cout << "Premier mot trouve  : aucun" << endl
             << "Plus grand mot      : aucun" << endl;

    cout << "\n--- COMPLEXITE TEMPORELLE (temps mesure) ---" << endl;
    cout << "Chargement          : " << ms(t0, t1) << " ms" << endl;
    cout << "Recherche           : " << ms(t2, t3) << " ms" << endl;
    cout << "Remplacement        : " << ms(t4, t5) << " ms" << endl;
    cout << "Total               : " << ms(t0, t5) << " ms" << endl;
    cout << "Theorie             : O(n x L)  (n = mots, L = longueur d'un mot)" << endl;

    cout << "\n--- COMPLEXITE SPATIALE (memoire) ---" << endl;
    cout << "Donnees (vector)    : " << octets / 1048576.0 << " Mo" << endl;
    cout << "Auxiliaire          : O(1)  (rechercher/remplacer n'allouent rien de plus que le mot)" << endl;
    cout << "Theorie             : O(n x L)" << endl;

    pause();
    return 0;
}
