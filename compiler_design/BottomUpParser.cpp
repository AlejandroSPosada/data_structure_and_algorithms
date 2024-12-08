/*
Code by AlejandroSPosada
Code based on 4.5 and 4.6 of Aho et al., Compilers: Principles, Techniques, and Tools (2nd Edition). 
1. Implement a function to compute CLOSURE(I) where I is set of items (see Section 4.6.2).
2. Compute the canonical LR(0) collection (see Section 4.6.2). Notice that you may compute the LR(0) automaton.
3. Compute the SLR-parsing table (Algorithm 4.46).
4. Implement the LR-parsing algorithm (Algorithm 4.44).

grammar input from terminal:
1
3
S SaT T
T TbF F
F cSd i
*/
#include <bits/stdc++.h>
#include <iomanip>
#define aFR() algorithmFirstRecursive(productionsMap, mapFirst, recolectorNT)
#include <stack>
using namespace std;

struct setItems {
  vector<pair<char, string>> start;
  vector<pair<char, string>> final;
  unordered_map<char, int> fuction;
};

struct parsingTable{
    map<pair<int, char>, pair<char, string>> reductions;
    map<pair<int, char>, pair<char, int>> actions;
    vector<char> symbols_minus;
    vector<char> symbols_mayus;
    vector<string> guia_r_producciones; // para en la parsing table saber que numero poner con la r
                                            // se contara a partir de la primera produccion ingresada (esa sera la 1)
    int tamaño_parsing_table;

    parsingTable() = default;
    void printParsingTable() {
        cout << "--------------------------------------------------------------------------------------\n";
        cout << "   State   |                  Action                  |             Go To            |\n";
        cout << "           ---------------------------------------------------------------------------\n";
        cout << setw(13) << setfill(' ') << right <<" | ";        

        symbols_minus.push_back('$');

        int e_a = 30/symbols_minus.size(); // espacios de action
        int e_gt = 24/(symbols_mayus.size()); // espacios de goto

        for(const char& symbol : symbols_minus){
            cout << setw(e_a) << setfill(' ') << left << symbol << "| ";
        }
        for(const char& symbol : symbols_mayus){
            cout << " " << setw(e_gt) << setfill(' ') << left << symbol << "| ";
        }       
        cout << "\n--------------------------------------------------------------------------------------\n";
        int contador = 0; // tambien sera el estado

        while(contador < tamaño_parsing_table) {
            cout << setw(10) << setfill(' ') << left << contador << " | ";
            for (const char& symbol : symbols_minus) {
                if (actions.find(make_pair(contador, symbol)) != actions.end()) {
                    const auto& action = actions[make_pair(contador, symbol)];

                    if (action.first == 'a'){
                        cout << setw(e_a) << setfill(' ') << left << "acc" << "| ";
                    }
                    else if (action.first == 'r') {
                        auto reduction = reductions[make_pair(contador, symbol)];
                        auto it = find(guia_r_producciones.begin(), guia_r_producciones.end(), reduction.second);
                        if (it != guia_r_producciones.end()) {
                            int index = distance(guia_r_producciones.begin(), it) + 1;
                            cout << setw(e_a) << setfill(' ') << left << "r" + to_string(index) << "| ";
                        }
                    } else {
                        cout << setw(e_a) << setfill(' ') << left << string(1, action.first) + to_string(action.second) << "| ";
                    }
                } else {
                    cout << setw(e_a) << setfill(' ') << left << " " << "| ";
                }
            }
            for (const char& symbol : symbols_mayus) {
                if (actions.find(make_pair(contador, symbol)) != actions.end()) {
                    const auto& action = actions[make_pair(contador, symbol)];
                    cout << setw(e_gt+1) << setfill(' ') << left << action.second << "| ";
                } else {
                    cout << setw(e_gt+1) << setfill(' ') << left << " " << "| ";
                }
            }
            contador++;
            cout << "\n";
        }
    }
};

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

void gotto(unordered_map<char, vector<string>>& productionsMap,
           vector<pair<char, string>>& start,
           vector<pair<char, string>>& final) {

    string inStart;
    char nonTerminal;
    vector<pair<char, string>> stackEvaluate = start;
    set<pair<char, string>> added;  // Usamos un set para evitar duplicados
    pair<char, string> currentItem;
    int indexDot;

    // Procesar ítems hasta que no haya más ítems por procesar
    while (!stackEvaluate.empty()) {
        currentItem = stackEvaluate.back();  // Tomamos el último ítem
        stackEvaluate.pop_back();  // Lo eliminamos de la lista

        inStart = currentItem.second;
        indexDot = inStart.find(".");
        
        // Asegurarnos de que el punto no está al final y que el siguiente símbolo es un no terminal
        if (indexDot + 1 < inStart.size() && isupper(inStart[indexDot + 1])) {
            nonTerminal = inStart[indexDot + 1];
            vector<string> productions = productionsMap[nonTerminal];

            // Procesamos las producciones del no terminal encontrado
            for (const string& production : productions) {
                string newProduction = "." + production;

                // Evitar duplicados
                if (added.find({nonTerminal, newProduction}) == added.end()) {
                    final.push_back(make_pair(nonTerminal, newProduction));
                    added.insert({nonTerminal, newProduction});
                    
                    // Solo agregar a stackEvaluate si no es el primer ítem
                    if (nonTerminal != production[0]) {
                        stackEvaluate.push_back(make_pair(nonTerminal, newProduction));
                    }
                }
            }
        }
    }
}

void gotToRight(vector<pair<char, string>>& startNext, char toChange, vector<pair<char, string>> allTogether) {
    for (const auto& production : allTogether) {
        string prod = production.second; // La producción en formato de cadena
        int dotPos = prod.find('.');  // Encontrar la posición del punto

        // Verificar si el punto está en una posición válida y si el siguiente símbolo es el `toChange`
        if (dotPos != -1 && dotPos + 1 < prod.size() && prod[dotPos + 1] == toChange) {
            // Mover el punto una posición a la derecha
            string newProd = prod.substr(0, dotPos) + prod[dotPos + 1] + '.' + prod.substr(dotPos + 2);
            
            // Agregar la nueva producción a startNext
            startNext.push_back(make_pair(production.first, newProd));
        }
    }
}

int isUniqueStart(const vector<setItems>& LR0, const vector<pair<char, string>>& start) {
    for (int i = 0; i < LR0.size(); ++i) {
        const setItems& currentSet = LR0[i];

        // Si los tamaños son diferentes, continuamos
        if (currentSet.start.size() != start.size()) continue;

        // Verificamos si todos los elementos de `start` están en `currentSet.start`
        bool isMatch = all_of(start.begin(), start.end(), 
            [&](const pair<char, string>& item) {
                return find(currentSet.start.begin(), currentSet.start.end(), item) != currentSet.start.end();
            });

        if (isMatch) {
            return i;  // Retorna el índice de coincidencia
        }
    }
    return -1;  // Si no se encontró una coincidencia, retorna -1
}

void constructLR0(vector<setItems>& LR0,
                  unordered_map<char, vector<string>>& productionsMap) {
    // Crear el estado inicial
    setItems zero;
    zero.start.push_back(make_pair('Q', ".S")); // Asumimos que 'S' es el símbolo inicial
    gotto(productionsMap, zero.start, zero.final);
    
    // Inicializar el primer estado en LR0
    LR0.push_back(zero);

    set<char> founds; // Usamos un set para controlar duplicados
    int indexDot;

    // Bucle principal para generar los estados
    for(int i = 0; i < LR0.size(); i++){
        
        // agregar ítems del estado actual
        vector<pair<char, string>> allTogether;
        allTogether.insert(allTogether.end(), LR0[i].start.begin(), LR0[i].start.end());
        allTogether.insert(allTogether.end(), LR0[i].final.begin(), LR0[i].final.end());
        
        // Procesar cada producción en el conjunto actual
        founds.clear();  // Limpiar los símbolos procesados
        for (const pair<char, string>& element : allTogether) {
            indexDot = element.second.find(".");
            
            // Verificamos si el símbolo después de '.' es válido
            if (indexDot + 1 < element.second.size()) {
                char nextSymbol = element.second[indexDot + 1];

                // Si el símbolo no ha sido procesado aún
                if (founds.find(nextSymbol) == founds.end()) {
                    setItems setItemsNext;
                    vector<pair<char, string>> startNext;
                    founds.insert(nextSymbol);  // Marcar el símbolo como procesado-

                    // Generar un nuevo estado con el punto desplazado
                    gotToRight(startNext, nextSymbol, allTogether);
                    setItemsNext.start = startNext;

                    // Completar `final` de este nuevo estado con `gotto`
                    gotto(productionsMap, setItemsNext.start, setItemsNext.final);

                    // Si el nuevo estado es único, lo agregamos
                     if (isUniqueStart(LR0, setItemsNext.start) == -1) {
                        LR0[i].fuction[nextSymbol] = LR0.size();
                        LR0.push_back(setItemsNext);
                    }
                    else LR0[i].fuction[nextSymbol] = isUniqueStart(LR0, setItemsNext.start);
                }
            }
        }
    }
}

void printLR0(const vector<setItems>& LR0) {
    for (size_t state = 0; state < LR0.size(); ++state) {
        const setItems& item = LR0[state];

        cout << "Estado " << state << ":\n";

        // Imprimir los ítems en 'start'
        cout << "  Start: ";
        for (const auto& p : item.start) {
            cout << "(" << p.first << " -> " << p.second << ") ";
        }
        cout << endl;

        // Imprimir los ítems en 'final'
        cout << "  Final: ";
        for (const auto& p : item.final) {
            cout << "(" << p.first << " -> " << p.second << ") ";
        }
        cout << endl;

        // Imprimir las transiciones en 'fuction'
        cout << "  Fuction: ";
        for (const auto& transition : item.fuction) {
            cout << "(" << transition.first << " -> Estado " << transition.second << ") ";
        }
        cout << endl;

        cout << "----------------------\n";
    }
}

void buildParsingTable( vector<setItems>& LR0, parsingTable& table,
                        unordered_map<char, set<char>> mapFollow, unordered_map<char, 
                        vector<string>> productionsMap) {

    table.tamaño_parsing_table = LR0.size();

    // Itera sobre todos los estados
    char myChar;
    int myInt;
    string myString;
    set<char> mySetChar;  // Corregir punto y coma faltante

    for (const auto& pair : productionsMap) {
        if (isupper(pair.first)) {
            table.symbols_mayus.push_back(pair.first);
        }
        for (const auto& production : pair.second) {
            for (char symbol : production) {
                if (islower(symbol)) {
                    table.symbols_minus.push_back(symbol);
                }
            }
        }
    }

    

    for (int i = 0; i < LR0.size(); ++i) {
        setItems& state = LR0[i];

        // Procesar las transiciones
        for(const pair<char, int>& fuction : state.fuction){
            myChar = fuction.first;
            myInt = fuction.second;

            if(isupper(myChar)) {
                table.actions[make_pair(i, myChar)] = make_pair('g', myInt);
            } else {
                table.actions[make_pair(i, myChar)] = make_pair('s', myInt);
            }
        }

        // Procesar las reducciones
        vector<pair<char, string>> allTogether;
        allTogether.insert(allTogether.end(), LR0[i].start.begin(), LR0[i].start.end());
        allTogether.insert(allTogether.end(), LR0[i].final.begin(), LR0[i].final.end());

        for(const pair<char, string>& element : allTogether) {
            myChar = element.first;
            myString = element.second;

            // Si el punto está al final y la producción es la final de la gramática
            if(myString.back() == '.' && myString[myString.size()-2] == 'S') {
                table.actions[make_pair(i, '$')] = make_pair('a', -1);  // Acción de aceptación
            } 
            // Si el punto está al final, realiza una reducción
            else if(myString.back() == '.') {
                myString.erase(myString.size() - 1);  // Eliminar el punto al final de myString

                mySetChar = mapFollow[myChar];
                for (const char& follow : mySetChar) {
                    // Reducción para los elementos del Follow
                    table.actions[make_pair(i, follow)] = make_pair('r', -1);
                    table.reductions[make_pair(i, follow)] = make_pair(myChar, myString);
                }
            }
        }
    }
}

void predictiveParser(parsingTable& myTable, string input1) {
    cout << "\n\nMoves made by a predictive parser on input: " << input1 << endl;
    cout << "------------------------------------------------------------------\n";
    cout << "   |   Stack   |    Symbols    |    Input    |       Action      |\n";
    cout << "------------------------------------------------------------------\n";

    // Input con el símbolo de fin de cadena '$'
    string input2 = input1 + "$";
    string matched = "";  // Para los símbolos que se han hecho match
    string action = "";
    std::stack<int> stack;  // Pila para los estados
    stack.push(0);  // El estado inicial es 0
    char terminal, nonTerminal;
    int index;

    int contador = 1;

    // Necesitamos imprimir los estados en la pila (que son enteros)
    std::stack<int> tempStack = stack;
    string stackContent = "";
    while (!tempStack.empty()) {
        stackContent = to_string(tempStack.top()) + " " + stackContent;
        tempStack.pop();
    }
    // Imprimir estado inicial
    cout << setw(2) << setfill(' ') << left << contador << " |";
    contador++;
    cout << " " << setw(9) << setfill(' ') << left << stackContent << " | ";
    cout << setw(11) << setfill(' ') << left << matched << "   |";
    cout << setw(12) << setfill(' ') << right << input2 << " | ";

    // Comienza el proceso de análisis
    while (!stack.empty()) {
        int currentState = stack.top();  // Estado en la cima de la pila
        terminal = input2[0];  // Primer terminal del input
        nonTerminal = stack.top();  // El no terminal en el estado

        // Acciones en función del tipo de entrada
        if (myTable.actions.find(make_pair(currentState, terminal)) != myTable.actions.end()) {
            pair<char, int> actionPair = myTable.actions[make_pair(currentState, terminal)];
            
            if (actionPair.first == 's') {  // Shift
                stack.push(actionPair.second);  // Empuja el nuevo estado
                action = "shift to state " + to_string(actionPair.second);
                matched = matched + input2[0];  // Símbolo que se ha hecho match
                input2 = input2.substr(1);  // Avanza en el input 
            }
            else if (actionPair.first == 'r') {  // Reduce
                // Usar la producción en reductions
                pair<char, string> reduction = myTable.reductions[make_pair(currentState, terminal)];
                nonTerminal = reduction.first;
                string production = reduction.second;
                action = "reduce " + string(1, nonTerminal) + " -> " + production;
                matched = matched.substr(0, matched.size() - production.size()) + string(1, nonTerminal);  // Símbolo que se ha hecho match

                // Quita los símbolos de la pila según la producción
                for (int i = 0; i < production.size(); ++i) {
                    stack.pop();  // Elimina los símbolos de la pila
                }

                // Empuja el estado de goto
                int nextState = myTable.actions[make_pair(stack.top(), nonTerminal)].second;
                stack.push(nextState);
            }
            else if (actionPair.first == 'a') {  // Accept
                action = "accept";
                break;
            }
            else if (actionPair.first == 'g') {  // Goto
                // El goto solo mueve el estado
                stack.push(actionPair.second);
                action = "goto state " + to_string(actionPair.second);
            }

        } else {
            // Si no existe una acción válida
            cout << "Input not valid!" << endl;
            break;
        }

        
        // Imprimir el contenido de la pila
        tempStack = stack;
        stackContent = "";
        while (!tempStack.empty()) {
            stackContent = to_string(tempStack.top()) + " " + stackContent;
            tempStack.pop();
        }

        

        cout << setw(17) << setfill(' ') << left << action << " |\n";
        // Imprimir el estado después de cada acción
        cout << setw(2) << setfill(' ') << left << contador << " |";
        contador++;
        cout << " " << setw(9) << setfill(' ') << left << stackContent << " | ";
        cout << setw(11) << setfill(' ') << left << matched << "   |";
        cout << setw(12) << setfill(' ') << right << input2 << " | ";
    }
    cout << setw(17) << setfill(' ') << left << "accept" << " |\n";
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
        vector<string> producciones_para_parsing_table;
    
        for (int i = 0; i < numNonTerminals; ++i) {
            string line;
            getline(cin, line); // Leer la línea completa
            istringstream iss(line);
            char nonTerminal;
            iss >> nonTerminal; // Leer el no terminal (carácter)
            string production;
            while (iss >> production) {
                productionsMap[nonTerminal].push_back(production); // Agregar producción al vector
                producciones_para_parsing_table.push_back(production);
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
        vector<setItems> LR0;
        constructLR0(LR0, productionsMap);
        parsingTable myParsingTable;
        myParsingTable.guia_r_producciones = producciones_para_parsing_table;
        buildParsingTable(LR0, myParsingTable, mapFollow, productionsMap);
        myParsingTable.printParsingTable();
        predictiveParser(myParsingTable, "ibiai");
    }
    return 0;
}
