#include <bits/stdc++.h>
#define aFR() algorithmFirstRecursive(productionsMap, mapFirst, recolectorNT)
using namespace std;

void imprimirMapVector(unordered_map<char, vector<string>> mM){
    for (const auto &pair : mM) {
        cout << pair.first << " -> ";
        for (auto it = pair.second.begin(); it != pair.second.end(); ++it) {
            cout << *it;
            if (next(it) != pair.second.end()) cout << ", ";
        }
        cout << endl;
    }
}

void imprimirMapChar(unordered_map<char, set<char>> mM, string word){
    for (const auto &pair : mM) {
        if(islower(pair.first)) continue;
        cout << word << "(" << pair.first << ")" << " = " << "{";
        for (auto it = pair.second.begin(); it != pair.second.end(); ++it) {
            cout << *it;
            if (next(it) != pair.second.end()) cout << ", ";
        }
        cout << "}";
        cout << endl;
    }
}

void algorithmFirst(    unordered_map<char, vector<string>>& productionsMap, 
                        unordered_map<char, set<char>>& mapFirst) {
    bool changed = true;
    
    while (changed) {
        changed = false;

        for (const auto& pair : productionsMap) {
            char noTerminal = pair.first;

            for (const auto& production : pair.second) {
                for (int i = 0; i < production.size(); ++i) {
                    char symbol = production[i];

                    // Si el símbolo es un terminal, añadirlo al conjunto First y salir del bucle
                    if (islower(symbol)) {
                        changed |= mapFirst[noTerminal].insert(symbol).second;
                        mapFirst[symbol].insert(symbol).second;
                        break;
                    }
                    
                    // Si el símbolo es un no terminal
                    if (isupper(symbol)) {
                        // Añadir First(symbol) al conjunto First(noTerminal), excepto 'e'
                        for (char firstSymbol : mapFirst[symbol]) {
                            if (firstSymbol != 'e') {
                                changed |= mapFirst[noTerminal].insert(firstSymbol).second;
                            }
                        }

                        // Si el símbolo no tiene 'e' en su First, detener el bucle
                        if (mapFirst[symbol].count('e') == 0) break;
                    }

                    // Si llegamos aquí, significa que todos los símbolos hasta este punto
                    // tienen 'e' en su conjunto First, por lo que añadimos 'e' si es el último símbolo
                    if (i == production.size() - 1) {
                        changed |= mapFirst[noTerminal].insert('e').second;
                    }
                }
            }
        }
    }
}

// PARTE CÓDIGO PARA ENCONTRAR FOLLOW
void algorithmFollow(   unordered_map<char, vector<string>>& productionsMap, 
                        unordered_map<char, set<char>>& mapFirst, 
                        unordered_map<char, set<char>>& mapFollow){
    vector<char> recolectorNT;
    int aux;
    bool find;
    mapFollow['S'].insert('$'); 
    for(const auto& pair : productionsMap){
        for(const auto& vectorElement : pair.second){
            aux = vectorElement.size();
            for(int i = 0; i < aux-1 && aux > 1; i++){
                if(islower(vectorElement[i]) || mapFirst[vectorElement[i]].count('e') == 0) recolectorNT.clear();
                if(isupper(vectorElement[i]) && i < aux-1) recolectorNT.push_back(vectorElement[i]);
                
                //if cSd, d belongs to the follows of S
                if(islower(vectorElement[i+1]) && isupper(vectorElement[i])){
                    for(const char& groupNT : recolectorNT){
                        mapFollow[groupNT].insert(vectorElement[i+1]);
                    }
                    recolectorNT.clear();
                }
                // If there is a production A -> A(NT)B, all first(B) but 'e' are in Follow(NT)
                else if(isupper(vectorElement[i]) && isupper(vectorElement[i+1])){
                    for(const auto& first : mapFirst[vectorElement[i+1]]){
                        for(const char& groupNT : recolectorNT){
                            if(first != 'e')
                            mapFollow[groupNT].insert(first);
                        }
                    }
                }
            }
            recolectorNT.clear();
        }
    }
    for(int i = 0; i < 20; i++){
        for(const auto& pair : productionsMap){
            for(const auto& vectorElement : pair.second){
                aux = vectorElement.size();
                // If there is a production A -> a(NT), all follow in A are in NT
                if(isupper(vectorElement.back())){
                    for(const auto& follow : mapFollow[pair.first])
                    mapFollow[vectorElement[aux-1]].insert(follow);
                }
                // If there is a production A -> a(NT)B, where First(B) has 'e', then all follow(A) are in Follow(NT)
                if (aux < 2 || islower(vectorElement.back()) || islower(vectorElement[aux - 2]) || 
                mapFirst[vectorElement.back()].count('e') == 0) continue;

                recolectorNT.push_back(vectorElement[aux-2]);
                for(int k = aux-2; k > 0; k--){
                    if(mapFirst[vectorElement[k]].count('e') == 1 && isupper(vectorElement[k])){
                        recolectorNT.push_back(vectorElement[k-1]);
                    }
                    else break;
                }
                for(const auto& follow : mapFollow[pair.first]){
                    for(const char& groupNT : recolectorNT){
                        if(follow != 'e')
                        mapFollow[groupNT].insert(follow);
                    }
                }
                recolectorNT.clear();
            }
        }
    }
}


int main() {
    int times;
    cin >> times;
    cin.ignore(); // Ignorar el salto de línea
    for(int j = 0; j < times; j++){
        int numNonTerminals;
        cin >> numNonTerminals; // Leer la cantidad de no terminales
        cin.ignore(); // Ignorar el salto de línea
        unordered_map<char, vector<string>> productionsMap;
        unordered_map<char, set<char>> mapFirst;
        unordered_map<char, set<char>> mapFollow;
    
        for (int i = 0; i < numNonTerminals; ++i) {
            string line;
            getline(cin, line); // Leer la línea completa
            istringstream iss(line);
            char nonTerminal;
            iss >> nonTerminal; // Leer el no terminal (carácter)
            string production;
            while (iss >> production) {
                productionsMap[nonTerminal].push_back(production); // Agregar producción al vector
            }
        }

        // Meto todos los no terminales en los maps de first y follow
        for(const auto &pair: productionsMap){
            mapFirst[pair.first];
            mapFollow[pair.first];
        }
        algorithmFirst(productionsMap, mapFirst);
        imprimirMapChar(mapFirst, "First");
        algorithmFollow(productionsMap, mapFirst, mapFollow);
        imprimirMapChar(mapFollow, "Follow");
    }
    return 0;
}