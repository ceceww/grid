#include "Grid.h"

// Set width and height to 0
Grid::Grid(){
	m = 0;
	n = 0;
}

Grid::~Grid(){
}

void Grid::loadFromFile(const std::string& filename){
	// Initialise variables
	std::string line; // The current line being read
	std::string grid_string=""; // Stores a string representation of the grid
	std::ifstream myfile(filename);  // Load the file
	int lineNum = 1;
	int height = 0;
	int width = 0;
	int startX, startY, goalX, goalY; // x and y co-ordinate values for start and goal
	// For checking whether start and goal are any of the grid co-ordinates
	bool startFound = false;
	bool goalFound = false;
	try {
		if (myfile.is_open()) {
			while (getline(myfile, line)) // Read each line one by one
			{
				if (line.empty()) { // If line is empty, signal error
					throw "Error loading grid description.";
				}
				if ((lineNum == 1) || (lineNum == 2)) { // If line is the first or second line read
					// Get co-ordinates between parentheses
					std::string co_ords = line.substr(line.find("(") + 1);
					co_ords.erase(std::remove(co_ords.begin(), co_ords.end(), ')'), co_ords.end());
					// Left with string representation of x and y co-ords
					std::string xStr = co_ords.substr(0, co_ords.find(","));
					std::string yStr = co_ords.substr(co_ords.find(",") + 1);
					// Convert to ints
					int x = atoi(xStr.c_str());
					int y = atoi(yStr.c_str());
					// If it's the first line and contains "START" keyword
					if ((lineNum == 1) && (line.find("START") != std::string::npos)) {
						// Set the start co-ord from these points
						startX = x;
						startY = y;
						setStart(startX, startY);
					}
					// If it's the second line and contains "GOAL" keyword
					else if ((lineNum == 2) && (line.find("GOAL") != std::string::npos)) {
						// Set the goal co-ord from these points
						goalX = x;
						goalY = y;
						setGoal(goalX, goalY);
					}
					else { // Otherwise there's something wrong with reading START and/or GOAL co-ordinates
						throw "Error loading grid description."; // Signal error
					}
				}
				else { // lineNum>2 contains a grid row
					grid_string.append(line + '|'); // '|' represents newline
					// If line is not the first row of grid and width is different from the previous row's width
					if ((width != 0) && (width != line.size())) {
						throw "Error loading grid description."; // Signal error
					}
					width = line.size(); // Update width to current line width. Should remain constant otherwise exception thrown in previous block
					height++; // After each row read, update height to height+1
				}
				lineNum++; // Line number is incremented for every line read including non-grid lines
			}
			myfile.close();
			n = height;
			m = width;
			// For setting the co-ordinates of each point
			int x = 0; // Limit is m-1
			int y = n - 1;
			// Transform grid_string into the grid by iterating over it
			for (int i = 0; i < grid_string.size(); i++) {
				if (grid_string[i] == 'o') { // If character read is 'o'
					CO co_ord = coord(x, y); // Create a co-ordinate out of x and y 
					// See if the co-ordinate (x,y) is start or goal
					if (isStart(x, y)) {
						startFound = true; // Update if co-ordinate is same as start
					}
					if (isGoal(x, y)) {
						goalFound = true; // Update if co-ordinate is same as goal
					}
					grid.insert({ co_ord,'o' }); // Insert the co-ordinate and unblocked char 'o' as tuple
					x++; // Increment x 
				}
				else if (grid_string[i] == 'x') { // If character read is 'x'
					CO co_ord = coord(x, y); // Create a co-ordinate out of x and y
					if (isStart(x, y)) {
						startFound = true; // Update if co-ordinate is same as start
					}
					if (isGoal(x, y)) {
						goalFound = true; // Update if co-ordinate is same as goal
					}
					grid.insert({ co_ord, 'x' }); // Insert the co-ordinate and blocked char 'x' as tuple
					x++; // Increment x
				}
				else if (grid_string[i] == '|') { // If character read is '|' this indicates a new row of grid
					x = 0; // Reset x
					y--; // Decrement y for new row
				}
				else { // If no valid character read
					throw "Error loading grid description."; // Signal error
				}
			}
			if ((!startFound) || (!goalFound)) { // If start or goal co-ordinates are not points of the grid
				throw "Position outside of the range of the grid."; // Signal error
			}
		}
			else throw "Error loading grid description."; // If there was a problem opening file
	}
	catch (const char* e) { std::cout << e << std::endl; exit(1); } // Catch and print out the error signals
}

// Checks whether the values in start tuple are the same as x and y
bool Grid::isStart(int x,int y) const{
	bool isStart=false;
	if ((std::get<0>(start) == x) && (std::get<1>(start) == y)) {
		isStart = true;
	}
	return isStart;
}

// Checks whether the values in goal tuple are the same as x and y
bool Grid::isGoal(int x,int y) const{
	bool isGoal = false;
	if ((std::get<0>(goal) == x) && (std::get<1>(goal) == y)) {
		isGoal = true;
	}
	return isGoal;
}

// Checks whether co-ordinate x,y is blocked
bool Grid::blocked(int x, int y) const{
	bool isBlocked=false;
	CO co_ord = std::make_tuple(x, y);
	if (grid.at(co_ord) == 'x') {
		isBlocked = true;
	}
	return isBlocked;
}

// Makes a tuple out of ints x and y to be the start co-ordinate
void Grid::setStart(int x, int y){
	start = coord(x, y);
}

// Makes a tuple out of ints x and y to be the goal co-ordinate
void Grid::setGoal(int x, int y){
	goal = coord(x, y);
}

void Grid::unblock(int x, int y){
	CO co_ord = coord(x, y); // Make a tuple co-ord out of x and y
	grid.at(co_ord) = 'o'; // Set char at co_ord to be 'o'
}

void Grid::block(int x, int y){
	CO co_ord = coord(x, y); // Make a tuple co-ord out of x and y
	grid.at(co_ord) = 'x'; // Set char at co_ord to be 'x'
}

bool Grid::existsPath(){
	bool existsPath = false;
	try {
		existsPath = findPath(std::get<0>(start), std::get<1>(start)); // Find the path between start and goal
		if (existsPath == false) { // if existsPath returns false after calling findPath
			throw "No path from start to goal.";
		}
	}
	catch (const char* e) { std::cout << e << std::endl; return existsPath; } // Catch and print out the error signals, return false
	return existsPath; // Returns if path is found
}

void Grid::outputPath(std::ostream& o){
	// Vector of all co-ordinates of grid
	std::vector<CO> keys;
	// Add just the co-ordinates of grid to 'keys' vector
	for (std::map<CO, char>::iterator i = grid.begin(); i != grid.end(); i++) {
		keys.push_back(i->first);
	}
	// Sort the vector so we can output the grid co-ordinates in correct order
	// First, sort by y co-ordinate descending
	std::sort(keys.begin(), keys.end(), [](CO &lhs, CO &rhs) {
	return (std::get<1>(lhs) > std::get<1>(rhs));	 });
	// Then, sort each row individually so x co-ordinate is ascending.
	for (int i = 0; i < n; i++) {
		std::sort(keys.begin()+(i*m), keys.begin() + ((i*m) + m));
	}
	int widthCount = 0;
	// Print the grid by iterating over the ordered vector of co-ordinates
	for (std::vector<CO>::iterator it = keys.begin(); it != keys.end(); it++){
		int x = std::get<0>(*it); // Get x any y components of co-ordinate
		int y = std::get<1>(*it);
		if (widthCount == m) { // If all the character in a row have been printed
			o << "\n"; // Print a new line
			widthCount = 0; // Reset char row count to 0
		}
		if (isGoal(x, y)) {
			o << "g";
		}
		else if (isStart(x, y)) {
			o << "s";
		}
		// If the co-ordinate of keys vector is in path
		else if (std::find(path.begin(), path.end(), *it) != path.end()) {
			o << "*"; 
		}
		else {
			o << grid.at(*it); // Prints the original character 'o' or 'x' of the grid.
		}
		widthCount++; // Increment every time a character is printed.
	}
	o << "\n";
}

void Grid::outputDirections(std::ostream& o){
	std::string facing = ""; // Currently facing one of North, South, East or West
	std::string direction = ""; // The instruction/direction string
	std::vector<std::string> directions; // Stores a list of directions.
	for (std::vector<CO>::iterator it = path.begin(); it != path.end(); it++) {
		int x = std::get<0>(*it); // Convert current co-ordinate to x and y components.
		int y = std::get<1>(*it); 
		auto nextIt = std::next(it, 1); // Next co-ordinate
		int nextX = std::get<0>(*nextIt); // Convert next co-ordinate to x and y components.
		int nextY = std::get<1>(*nextIt);

		// If start and goal are the same
		if (isStart(x, y) && isGoal(x, y)) {
			directions.push_back("You have arrived at your destination.");
		}
		// If it's the start co-ordinate, the direction must be in the form "Move from ... to ... "
		// So push back first part of this string, that will be appended to later on
		else if (isStart(x, y)) {
			direction = "Move from (" + std::to_string(x) + "," + std::to_string(y) + ")";
			directions.push_back(direction);
		}
		else if ((nextX == x + 1) && (nextY == y)) { // If turning East
			if ((facing != "East") && (facing != "")){ // Checks that where we're facing is different from before
				// Complete the previous direction
				directions.back() = directions.back() + " to (" + std::to_string(x) + "," + std::to_string(y) + ").";
				directions.push_back("Turn East."); // Just turned East (a new direction) so add this to directions
				direction = "Move from (" + std::to_string(x) + "," + std::to_string(y) + ")"; // Start a new direction string
				directions.push_back(direction); // And add it to directions (will be appended to later on)
			}
			facing = "East"; // Now we're facing East, so update
		}
		else if ((nextX == x - 1) && (nextY == y)) { // If turning West
			if ((facing != "West") && (facing != "")) { // Checks that where we're facing is different from before
				// Complete the previous direction
				directions.back() = directions.back() + " to (" + std::to_string(x) + "," + std::to_string(y) + ").";
				directions.push_back("Turn West."); // Just turned West (a new direction) so add this to directions
				direction = "Move from (" + std::to_string(x) + "," + std::to_string(y) + ")";  // Start a new direction string
				directions.push_back(direction); // And add it to directions (will be appended to later on)
			}
			facing = "West"; // Now we're facing West, so update
		}
		else if ((nextX == x) && (nextY == y+1)) { // If turning North
			if ((facing != "North") && (facing != "")) { // Checks that where we're facing is different from before
				// Complete the previous direction
				directions.back() = directions.back() + " to (" + std::to_string(x) + "," + std::to_string(y) + ").";
				directions.push_back("Turn North."); // Just turned North (a new direction) so add this to directions
				direction = "Move from (" + std::to_string(x) + "," + std::to_string(y) + ")"; // Start a new direction string
				directions.push_back(direction); // And add it to directions (will be appended to later on)
			}
			facing = "North"; // Now we're facing North, so update	
		}
		else if ((nextX == x) && (nextY == y - 1)) { // If turning South
			if ((facing != "South") && (facing != "")) { // Checks that where we're facing is different from before
				// Complete the previous direction
				directions.back() = directions.back() + " to (" + std::to_string(x) + "," + std::to_string(y) + ").";
				directions.push_back("Turn South."); // Just turned South (a new direction) so add this to directions
				direction = "Move from (" + std::to_string(x) + "," + std::to_string(y) + ")"; // Start a new direction string
				directions.push_back(direction); // And add it to directions (will be appended to later on)
			}
			facing = "South"; // Now we're facing South, so update
		}
		else if (isGoal(x, y)) { // If co-ordinate is the goal
			// Complete the previous direction
			directions.back() = directions.back() + " to (" + std::to_string(x) + "," + std::to_string(y) + ")."; 
			directions.push_back("You have arrived at your destination."); // This is the final direction
		}
	}
	// Output directions
	for (std::vector<std::string>::iterator it = directions.begin(); it != directions.end(); it++) {
		o << *it << '\n';
	}
}

CO Grid::coord(int x, int y) {
	CO co_ord = std::make_tuple(x, y); // Creates a tuple<int,int> out of x and y integer components
	return co_ord;
}

// Recursive function to find path
// Exceptions thrown need to be caught where findPath is called
bool Grid::findPath(int x, int y) {
	// If x,y is the start co-ord, check that start is not blocked
	if (isStart(x, y)) {
		if (blocked(x, y)) {
			throw "No path from start to goal";
		}
	}

	if (isStart(x, y) && isGoal(x, y)) { // If start and goal points are the same
		CO co_ord = coord(x, y); // Make a co-ordinate out of x and y
		path.push_back(co_ord); // Add this single point to path
		return true;
	}

	CO co_ord = coord(x, y); // Make a co-ordinate out of x and y
	std::map<CO, char>::iterator it; // Iterator to look over the grid
	it = grid.find(co_ord);
	if (it == grid.end())	return false; // If co-ordinate is not inside grid, return false
	if (isGoal(x, y)) {
		if (blocked(x, y)) {
			throw "No path from start to goal";
		}
		path.push_back(co_ord);
		return true; // If co_ord is goal return true
	}
	if (blocked(x, y))	return false; // If co_ord is blocked return false
  // Check if we've already stored this point
	std::vector<CO>::iterator itt = std::find(path.begin(), path.end(), co_ord);
	if (itt != path.end())	return false;
	path.push_back(co_ord);
	if ((findPath(x - 1, y) == true) == true)	return true;
	if ((findPath(x + 1, y) == true) == true)	return true;
	if ((findPath(x, y - 1) == true) == true)	return true;
	if ((findPath(x, y + 1) == true) == true)	return true;
	// If we go back on ourselves
	path.pop_back();
	return false;
}