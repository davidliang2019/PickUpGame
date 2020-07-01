//
//	MovementGraph.h
//


#ifndef MOVEMENTGRAPH_H
#define MOVEMENTGRAPH_H

#include "ObjLibrary/Vector3.h"
#include "UpdatablePriorityQueue.h"
#include "World.h"
#include "Disk.h"
#include <vector>
#include <stack>

class MovementGraph {
private:
	// create two structs for std::vector<Node> node_array
	struct Link
	{
		unsigned int node_id;
		float weight;
	};

	struct Node {
		ObjLibrary::Vector3 position;
		unsigned int disk_id;
		std::vector<Link> neighbor;
	};

	// create a struct for std::vector<NodeList> disk_node_list
	struct NodeList {
		std::vector<unsigned int> node_list;
		void add(const unsigned int& node_id) {
			node_list.push_back(node_id);
		}
	};

	// create a struct for std::vector<SearchData> search_data_array
	struct SearchData {
		unsigned int best_previous_node_id;
		float best_cost_to_here;
		float heuristic;
		float priority;
	};

	// create a enum to tell the function which search methor are used
	enum Search { AStar, MM };

	// initialize the class when the game starts
public:
	void init(const World& world);

	// generate movement graph when the game initializes
	// use the exact same name in the suggested approach
private:
	void addToGraph(const World& world,
		const unsigned int& i,
		const unsigned int& j);
	unsigned int addNode(ObjLibrary::Vector3 position,
		unsigned int disk_id);
	void addLink(const unsigned int& node_i,
		const unsigned int& node_j,
		const float& weight_ij);
	float calculateWeightBetweenDisks(const unsigned int& node_i,
		const unsigned int& node_j,
		const unsigned int& disk_type_i,
		const unsigned int& disk_type_j);
	float calculateWeightSameDisks(const unsigned int& node_i,
		const unsigned int& node_k,
		const Disk& disk_i);
	ObjLibrary::Vector3 calculateNodePosition(const Disk& disk_i,
		const Disk& disk_j);


	// common functions for both search algorithm
public:
	void pathFinding();
	void updatePath(const unsigned int& source_id);
	void displayWhen(bool is_true);		// label the ring 0 display when i = 0
private:
	// initialize arrays and variable
	void initializeSourceAndGoal(unsigned int& source_id,
		unsigned int& goal_id);
	std::vector<SearchData> initializeSearchData(const unsigned int& source_id,
		const unsigned int& goal_id, Search search_type);

	// verification
	bool checkAtLeast3NodesAway(const unsigned int& source_id,
		const unsigned int& goal_id);
	bool isNodeInCloseList(unsigned int node_id, std::vector<unsigned int>& close_list);

	// sub-operation
	void splitNode(std::vector<SearchData>& search_data_array,
		UpdatablePriorityQueue<float>& open_list,
		std::vector<unsigned int>& close_list,
		Search search_type);
	float updatePriority(float cost, float heuristic, Search search_type);
	
	// functions for A Star search 
	void AStarSearch(const unsigned int& source_id,
					const unsigned int& goal_id);
	void drawAStarSphere() const;

	// functions for MM search
	void MMSearch(const unsigned int& source_id,
		const unsigned int& goal_id);
	void drawMMSphere() const;

	//output functions
public:
	void drawPath() const;
	void drawSphere() const;
	ObjLibrary::Vector3 getPosition(unsigned int node_id);
	unsigned int getDiskId(unsigned int node_id);
	std::stack<unsigned int> getPath();
	
	// member variables
private:
	std::stack<unsigned int> path;
	std::vector<Node> node_array;
	std::vector<NodeList> disk_node_list;

	// store the information for ring0
	std::vector<SearchData> search_data_from_source_display;
	std::vector<SearchData> search_data_from_goal_display;
};

#endif
