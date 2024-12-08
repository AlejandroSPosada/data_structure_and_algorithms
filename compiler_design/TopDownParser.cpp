/* 
Code by AlejandroSPosada
Code based on Compilers: Principles, Techniques, and Tools (2nd Edition).
1. Verify whether the grammar is LL(1).
2. Implement a function to compute the First set of a string.
3. Compute the parsing table (Algorithm 4.31).
4. Implement the table-driven predictive parsing algorithm (Algorithm 4.34).

input from terminal example:
1
5
S TZ
Z aTZ e
T FX 
X bFX e
F cSd f
*/
#include <bits/stdc++.h>
#include <iomanip>
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

bool disjointSet(const set<char>& firstSet, const set<char>& secondSet) {
    for (const char& element1 : firstSet) {
        if (secondSet.find(element1) != secondSet.end()) {
            return false;
        }
    }
    return true;
}

bool wonderIfIsLL1(unordered_map<char, vector<string>>& mapProductions, 
                   unordered_map<char, set<char>>& mapFirst, 
                   unordered_map<char, set<char>>& mapFollow) {
    for (const auto& nonTerminal : mapProductions) {
        const char& nt = nonTerminal.first;
        vector<string> productions = nonTerminal.second;
        // Whenever A -> B | C with B != C
        // 1. First(B) ∪ First(C) = ∅
        for (int i = 0; i < productions.size() - 1; i++) {
            for (int j = i + 1; j < productions.size(); j++) {
                bool disjoint = disjointSet(mapFirst[productions[i][0]], mapFirst[productions[j][0]]);
                if (!disjoint) {
                    return false;
                }
            }
        }
        // Whenever A -> B | C with B != C
        // 2. If 𝜖 existis in First(B), then  First(C) ∪ Follow(A) = ∅
        bool disjointMatriz[productions.size()] = { true };
        for(int i = 0; i < productions.size(); i++){
            if(!disjointSet(mapFollow[nt], mapFirst[productions[i][0]]))
            disjointMatriz[i] = 0;
        }

        for(int i = 0; i < productions.size(); i++){
            if(mapFirst[productions[i][0]].count('e') == 1){
                for(int j = 0; j < productions.size(); j++){
                    if(j != i && !disjointMatriz[j]) return false;
                }
            }
        }
    }
    return true;
}
void printParsingTable(map<pair<char, char>, int>& parsingTable, 
                       unordered_map<char, vector<string>>& productionsMap) {
    // Imprimimos un bloque de texto
    cout << "Parsing Table:\n";
    cout << "---------------------------------------------------\n";
    cout << "| Non-terminal | Terminal | Production Index     |\n";
    cout << "---------------------------------------------------\n";
    
    for (const auto& entry : parsingTable) {
        char nonTerminal = entry.first.first;
        char terminal = entry.first.second;
        int productionIndex = entry.second;
        if(productionIndex == -1) continue;
        cout << "| " << nonTerminal << "            | " 
                  << terminal << "        | ";
        
        // Si hay una producción asociada, imprímela
            if (productionIndex < productionsMap.at(nonTerminal).size()) {
                cout << nonTerminal << " -> ";
                cout << setw(15) << setfill(' ') << left << productionsMap[nonTerminal][productionIndex] << " |" << endl;
            } 
    }
    cout << "---------------------------------------------------\n";
}

void makeParsingTable(  unordered_map<char, vector<string>>& mapProductions,
                        unordered_map<char, set<char>>& mapFirst,
                        unordered_map<char, set<char>>& mapFollow,
                        map<pair<char, char>, int>& parsingTable){
    char nonTerminal, firstCharacter;
    vector<char> recolectorT;
    string production;
    vector<string> productionsVector;
    for(const auto& elementMapProductions : mapProductions){
        for(const string& p : elementMapProductions.second){
            for(int i = 0; i < p.size(); i++){
                if(islower(p[i]) && p[i] != 'e') recolectorT.push_back(p[i]);
            }
        }
    } 
    recolectorT.push_back('$');
    for(const auto& elementMapProductions : mapProductions){
        nonTerminal = elementMapProductions.first;
        for(const char& terminal : recolectorT){
            parsingTable[make_pair(nonTerminal, terminal)] = -1;
        }
    }
    // for each production A -> Prod of the grammar
    // 1. for each terminal a in First(Prod), add A -> Prod to M[A,a]
    for(const auto& elementMapProductions : mapProductions){
        nonTerminal = elementMapProductions.first;
        productionsVector = elementMapProductions.second;
        for(int i = 0; i < productionsVector.size(); i++){
            firstCharacter = productionsVector[i][0];
            for(const char& terminal : mapFirst[firstCharacter]){
                if(terminal == 'e' ) continue;
                parsingTable[make_pair(nonTerminal, terminal)] = i;
            }
        }
    }
    // for each production A -> Prod of the grammar
    // 2. If e is in First(Prod), then for each terminal b in follow(A), add
    // A -> Prod to M[A,b]. If e is in First(Prod) and $ is in Follow(A), add 
    // A -> Prod to M[A,$] as well.
    for(const auto& elementMapProductions : mapProductions){
        nonTerminal = elementMapProductions.first;
        productionsVector = elementMapProductions.second;
        for(int i = 0; i < productionsVector.size(); i++){
            firstCharacter = productionsVector[i][0];
            if(mapFirst[firstCharacter].count('e') == 1 ){
                for(const char& terminal : mapFollow[nonTerminal]){
                    parsingTable[make_pair(nonTerminal, terminal)] = i;
                }
            }
        }
    }
}


void predictiveParser(  map<pair<char, char>, int>& parsingTable,
                        unordered_map<char, vector<string>>& productionsMap,
                         string input1){
    cout << "\n\nMoves mady by a predictive parser on input: " << input1 << endl;
    cout << "----------------------------------------------------------------\n";
    cout << "|   Matched   |    Stack    |    Input    |       Action      |\n";
    cout << "----------------------------------------------------------------\n";
    string matched = "", stack = "S$", input2 = input1 + "$", action = "", production;
    char terminal, nonTerminal;
    int index;
    //cout << productionsMap['F'][1] << ", " << parsingTable[make_pair('F', 'f')] << endl;
    cout << "| " <<setw(11) << setfill(' ') << left << matched << " |";
    cout << setw(12) << setfill(' ') << right << stack << " |";
    cout << setw(12) << setfill(' ') << right << input2 << " | ";
    cout << setw(17) << setfill(' ') << left << action <<" | " << endl;
    while(stack[0] != '$'){
        if(stack[0] == input2[0]){
            action = "match " + string(1,stack[0]);
            matched = matched + string(1,stack[0]);
            input2 = input2.erase(0,1);
            stack = stack.erase(0,1);
        }
        else{
            terminal = input2[0];
            nonTerminal = stack[0];
            index = parsingTable[make_pair(nonTerminal, terminal)];
            if(index == -1){
                cout << "input not valid" << endl;
                break;
            }
            else{
                production = productionsMap[nonTerminal][index];
            }
            stack = stack.erase(0,1);
            if(production[0] != 'e') stack = production + stack;
            action = "output " + string(1, nonTerminal) + " -> " + production;
        } 
        cout << "| " <<setw(11) << setfill(' ') << left << matched << " |";
        cout << setw(12) << setfill(' ') << right << stack << " |";
        cout << setw(12) << setfill(' ') << right << input2 << " | ";
        cout << setw(17) << setfill(' ') << left << action <<" | " << endl;
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
        if(wonderIfIsLL1(productionsMap,mapFirst, mapFollow)) cout << "La gramatica es LL(1)" << endl;
        else{
            cout << "La gramatica no es LL(1)" << endl;
        }
        map<pair<char, char>, int> parsingTable;
        makeParsingTable(productionsMap, mapFirst, mapFollow, parsingTable);
        printParsingTable(parsingTable, productionsMap);
        predictiveParser(parsingTable, productionsMap, "fafbf");
    }
    return 0;
}
