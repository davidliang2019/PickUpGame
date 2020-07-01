//
//	MovementGraph.cpp
//

#include "MovementGraph.h"
#include "DiskType.h"
#include "Disk.h"
#include "GetGlut.h"
#include "UpdatablePriorityQueue.h"
#include <stack>
#include <algorithm>

using namespace std;
using namespace ObjLibrary;
namespace {
	const unsigned int NEVER_REACHED = 999999999;
	const float MAX_COST = 99999.9f;

	bool display_sphere = false;
	unsigned int meeting_node_id_display;
	float max_priority_from_source;
	float max_priority_from_goal;
	float min_priority_from_source;
	float min_priority_from_goal;
	int random1(int max)
	{
		float random01 = (rand() / (RAND_MAX + 1.0f));
		return (int)floor(random01 * max);
	}
}

void MovementGraph::init(const World& world)
{
	unsigned int disk_count = world.getDiskCount();
	node_array.clear();
	disk_node_list.clear();
	for (unsigned int i = 0; i < disk_count; i++)
		disk_node_list.push_back({});
	
	for (unsigned int i = 0; i < disk_count - 1; i++) {
		const Disk& disk_i = world.getDisk(i);
		for (unsigned int j = i + 1; j < disk_count; j++) {
			const Disk& disk_j = world.getDisk(j);
			double distance = disk_i.getPosition().getDistance(disk_j.getPosition());
			if (distance <= disk_i.getRadius() + disk_j.getRadius() + 0.01)
				addToGraph(world, i, j);
		}
	}
}

// following 6 functions are same in the suggested approach
// from line 48 to line 120
void MovementGraph::addToGraph(const World& world, const unsigned int& i, const unsigned int& j)
{
	const Disk& disk_i = world.getDisk(i);
	const Disk& disk_j = world.getDisk(j);
	Vector3 position_i = calculateNodePosition(disk_i, disk_j);
	Vector3 position_j = calculateNodePosition(disk_j, disk_i);
	unsigned int node_i_id = addNode(position_i, i);
	unsigned int node_j_id = addNode(position_j, j);

	float weight_ij = calculateWeightBetweenDisks(node_i_id, node_j_id,
		disk_i.getDiskType(), disk_j.getDiskType());
	addLink(node_i_id, node_j_id, weight_ij);

	for (unsigned int k = 0; k < disk_node_list[i].node_list.size(); k++) {
		unsigned int node_k_id = disk_node_list[i].node_list[k];
		float weight_ik = calculateWeightSameDisks(node_i_id, node_k_id, disk_i);
		addLink(node_i_id, node_k_id, weight_ik);
	}

	for (unsigned int k = 0; k < disk_node_list[j].node_list.size(); k++) {
		unsigned int node_k_id = disk_node_list[j].node_list[k];
		float weight_jk = calculateWeightSameDisks(node_j_id, node_k_id, disk_j);
		addLink(node_j_id, node_k_id, weight_jk);
	}

	disk_node_list[i].add(node_i_id);
	disk_node_list[j].add(node_j_id);
}

unsigned int MovementGraph::addNode(ObjLibrary::Vector3 position, unsigned int disk_id)
{
	node_array.push_back({ position, disk_id,{} });
	return node_array.size() - 1;
}

void MovementGraph::addLink(const unsigned int& node_i, const unsigned int& node_j, const float& weight_ij)
{
	node_array[node_i].neighbor.push_back({ node_j, weight_ij });
	node_array[node_j].neighbor.push_back({ node_i, weight_ij });
}

float MovementGraph::calculateWeightBetweenDisks(const unsigned int& node_i,
	const unsigned int& node_j,
	const unsigned int& disk_type_i,
	const unsigned int& disk_type_j)
{
	float distance = (float)node_array[node_i].position.getDistance(node_array[node_j].position);
	float i_factor = 1.0f / DiskType::getRingSpeedFactor(disk_type_i);
	float j_factor = 1.0f / DiskType::getRingSpeedFactor(disk_type_j);
	return distance * (i_factor + j_factor) * 0.5f;
}

float MovementGraph::calculateWeightSameDisks(const unsigned int& node_i,
	const unsigned int& node_k,
	const Disk& disk_i)
{
	Vector3 center_to_node_i = node_array[node_i].position - disk_i.getPosition();
	Vector3 center_to_node_k = node_array[node_k].position - disk_i.getPosition();
	double  arc_radians = center_to_node_i.getAngle(center_to_node_k);
	float arc_length = disk_i.getRadius() * (float)arc_radians;
	float i_factor = 1.0f / DiskType::getRingSpeedFactor(disk_i.getDiskType());
	return arc_length * i_factor;
}

Vector3 MovementGraph::calculateNodePosition(const Disk& disk_i, const Disk& disk_j)
{
	Vector3 center_i = disk_i.getPosition();
	Vector3 center_j = disk_j.getPosition();
	Vector3 longdirvector = center_j - center_i;
	Vector3 u = longdirvector.getNormalized();
	float distance = disk_i.getRadius() - 0.7f;
	return center_i + u * distance;
}

// the following three functions is common sub-function for both search

void MovementGraph::pathFinding()
{
	unsigned int source_id;
	unsigned int goal_id;
	initializeSourceAndGoal(source_id, goal_id);
	AStarSearch(source_id, goal_id);
	//MMSearch(source_id, goal_id);
}

void MovementGraph::updatePath(const unsigned int& source_id)
{
	unsigned int goal_id;
	bool initialize_source_and_goal = false;
	while (!initialize_source_and_goal) {
		goal_id = random1(node_array.size());
		assert(goal_id < node_array.size());
		initialize_source_and_goal = checkAtLeast3NodesAway(source_id, goal_id);
	}
	AStarSearch(source_id, goal_id);
	//MMSearch(source_id, goal_id);
}

void MovementGraph::displayWhen(bool is_true)
{
	if (is_true)
		display_sphere = true;
}

// the following two functions initialize array and node for both searches

void MovementGraph::initializeSourceAndGoal(unsigned int& source_id, unsigned int& goal_id)
{
	bool initialize_source_and_goal = false;
	while (!initialize_source_and_goal) {
		source_id = random1(node_array.size());
		assert(source_id < node_array.size());
		goal_id = random1(node_array.size());
		assert(goal_id < node_array.size());
		initialize_source_and_goal = checkAtLeast3NodesAway(source_id, goal_id);
	}
}

vector<MovementGraph::SearchData>
MovementGraph::initializeSearchData(const unsigned int& source_id,
									const unsigned int& goal_id,
									Search search_type)
{
	Vector3 goal = node_array[goal_id].position;
	vector<SearchData> search_data_array;
	for (unsigned int i = 0; i < node_array.size(); i++) {
		search_data_array.push_back({});
		search_data_array[i].best_previous_node_id = NEVER_REACHED;
		search_data_array[i].best_cost_to_here = MAX_COST;
		search_data_array[i].heuristic = (float)node_array[i].position.getDistance(goal);
		search_data_array[i].priority = updatePriority(search_data_array[i].best_cost_to_here,
			search_data_array[i].heuristic, search_type);
	}

	// initialize the source_id
	search_data_array[source_id].best_previous_node_id = source_id;
	search_data_array[source_id].best_cost_to_here = 0.0f;
	search_data_array[source_id].priority = updatePriority(search_data_array[source_id].best_cost_to_here,
		search_data_array[source_id].heuristic, search_type);
	return search_data_array;
}

// the following two functions verify nodes

bool MovementGraph::checkAtLeast3NodesAway(const unsigned int& source_id, const unsigned int& goal_id)
{
	if (source_id == goal_id)
		return false;
	for (unsigned int j = 0; j < node_array[goal_id].neighbor.size(); j++) {
		if (source_id == node_array[goal_id].neighbor[j].node_id)
			return false;
	}
	for (unsigned int i = 0; i < node_array[source_id].neighbor.size(); i++) {
		if (node_array[source_id].neighbor[i].node_id == goal_id)
			return false;
		for (unsigned int j = 0; j < node_array[goal_id].neighbor.size(); j++) {
			if (node_array[source_id].neighbor[i].node_id == node_array[goal_id].neighbor[j].node_id)
				return false;
		}
	}
	return true;
}

bool MovementGraph::isNodeInCloseList(unsigned int node_id, std::vector<unsigned int>& close_list)
{
	for (unsigned int i = 0; i < close_list.size(); i++)
		if (node_id == close_list[i])
			return true;
	return false;
}

// the following function outputs all the adjacent nodes from the current node
// and delete the current node from the open list
// and calculate their priority
// and insert or update in the open list
// and insert to the close list
void MovementGraph::splitNode(std::vector<SearchData>& search_data_array,
	UpdatablePriorityQueue<float>& open_list,
	std::vector<unsigned int>& close_list,
	Search search_type)
{
	// delete the current node from the open list
	unsigned int current_node_id = open_list.peekAndDequeue();

	//outputs all the adjacent nodes from the current node
	for (unsigned int i = 0; i < node_array[current_node_id].neighbor.size(); i++) {
		const unsigned int node_id = node_array[current_node_id].neighbor[i].node_id;
		const float weight = node_array[current_node_id].neighbor[i].weight;

		// check whether the neighbor is in the close list
		bool is_in_close_list = false;
		for (unsigned int j = 0; j < close_list.size(); j++)
			if (node_id == close_list[j])
				is_in_close_list = true;
		if (!is_in_close_list) {
			// update the priority of the neighbor
			float old_cost = search_data_array[node_id].best_cost_to_here;
			float new_cost = search_data_array[current_node_id].best_cost_to_here + weight;
			if (new_cost < old_cost) {
				search_data_array[node_id].best_previous_node_id = current_node_id;
				search_data_array[node_id].best_cost_to_here = new_cost;
				search_data_array[node_id].priority =
					updatePriority(new_cost, search_data_array[node_id].heuristic, search_type);
			}
			// insert the neighbor to the open list
			// or update the priority if the lower one has found
			float new_priority = search_data_array[node_id].priority;
			if (!open_list.isEnqueued(node_id))
				open_list.enqueue(node_id, new_priority);
			else if (open_list.getPriority(node_id) > new_priority)
				open_list.setPriority(node_id, new_priority);
		}
	}
	// insert to the close list
	close_list.push_back(current_node_id);
}

// select the update priority method
float MovementGraph::updatePriority(float cost, float heuristic, Search search_type)
{
	if (search_type == AStar)
		return cost + heuristic;
	else if (search_type == MM)
		return cost + max(cost, heuristic);
	else
		exit(1);
}

void MovementGraph::AStarSearch(const unsigned int& source_id,
	const unsigned int& goal_id)
{
	// initialize the arrays
	vector<SearchData> search_data_array = initializeSearchData(source_id, goal_id, AStar);
	UpdatablePriorityQueue<float> open_list(node_array.size());
	vector<unsigned int> close_list;

	// insert the starting node to the open list
	open_list.enqueue(source_id, search_data_array[source_id].priority);

	// the while loop will stop when the destination has been reached
	// or the open list is emtpy
	while (!open_list.isQueueEmpty()) {
		if (open_list.peek() == goal_id)
			break;
		splitNode(search_data_array, open_list, close_list, AStar);
	}

	// if the ring is ring0, copy the search data which will display
	// as spheres later
	if (display_sphere) {
		search_data_from_source_display.clear();
		search_data_from_source_display = search_data_array;
		max_priority_from_source = search_data_array[goal_id].priority;
		min_priority_from_source = search_data_array[source_id].priority;
		display_sphere = false;
	}

	// clear up the path stack
	while (!path.empty())
		path.pop();
	unsigned int current = goal_id;

	// fill up the stack from the destination node
	while (current != source_id) {
		path.push(current);
		current = search_data_array[current].best_previous_node_id;
	}
	path.push(current);
}

void MovementGraph::drawAStarSphere() const
{
	for (unsigned int k = 0; k < node_array.size(); k++) {
		glPushMatrix();
		glTranslatef((GLfloat)node_array[k].position.x,
			(GLfloat)node_array[k].position.y,
			(GLfloat)node_array[k].position.z);
		// draw the starting node
		if (search_data_from_source_display[k].best_previous_node_id == k) {
			glColor3f(0.0f, 1.0f, 1.0f);
			glutSolidSphere(3.0, 50, 50);
		}// draw the destination node
		else if (search_data_from_source_display[k].heuristic == 0.0f) {
			glColor3f(1.0f, 1.0f, 1.0f);
			glutSolidSphere(3.0, 50, 50);
		}// draw the all nodes have been reached
		else if (search_data_from_source_display[k].best_previous_node_id < NEVER_REACHED) {
			glColor3f(0.0f, 0.5f + 0.5f*(max_priority_from_source 
				- search_data_from_source_display[k].priority)
				/ (max_priority_from_source - min_priority_from_source), 1.0f);
			glutSolidSphere(1.5, 50, 50);
		}
		glPopMatrix();
	}
}

// the detail description is in the pdf file
void MovementGraph::MMSearch(const unsigned int& source_id, const unsigned int& goal_id)
{
	// initialize all arrays
	vector<SearchData> search_data_from_source = initializeSearchData(source_id, goal_id, MM);
	vector<SearchData> search_data_from_goal = initializeSearchData(goal_id, source_id, MM);
	UpdatablePriorityQueue<float> open_list_from_source(node_array.size());
	UpdatablePriorityQueue<float> open_list_from_goal(node_array.size());
	vector<unsigned int> close_list_from_source;
	vector<unsigned int> close_list_from_goal;
	unsigned int meeting_node_id;

	// insert the starting node to the open list
	open_list_from_source.enqueue(source_id, search_data_from_source[source_id].priority);
	open_list_from_goal.enqueue(goal_id, search_data_from_goal[goal_id].priority);

	// the while loop will stop when the destination has been reached
	// or the open list is emtpy
	while (!open_list_from_source.isQueueEmpty() && !open_list_from_goal.isQueueEmpty()) {
		if (isNodeInCloseList(open_list_from_source.peek(), close_list_from_goal)) {
			meeting_node_id = open_list_from_source.peek();
			break;
		}
		else if (isNodeInCloseList(open_list_from_goal.peek(), close_list_from_source)) {
			meeting_node_id = open_list_from_goal.peek();
			break;
		}

		// selection current node from the lower priority head of two open lists
		if (open_list_from_source.peekPriority() < open_list_from_goal.peekPriority())
			splitNode(search_data_from_source, open_list_from_source,
				close_list_from_source, MM);
		else
			splitNode(search_data_from_goal, open_list_from_goal,
				close_list_from_goal, MM);
	}

	// clear up the path stack
	while (!path.empty())
		path.pop();

	// fill up the invert path stack
	// from the meeting node to the destination node
	std::stack<unsigned int> invert_path;
	unsigned int current = meeting_node_id;
	while (current != goal_id) {
		invert_path.push(current);
		current = search_data_from_goal[current].best_previous_node_id;
	}
	invert_path.push(current);

	// fill up the path stack from the invert path stack
	// from the destination node to previous node of the meeting node
	while (!invert_path.empty()) {
		path.push(invert_path.top());
		invert_path.pop();
	}

	// fill up the invert path stack
	// from the meeting node to the starting node
	current = search_data_from_source[meeting_node_id].best_previous_node_id;
	while (current != source_id) {
		path.push(current);
		current = search_data_from_source[current].best_previous_node_id;
	}
	path.push(current);

	// if the ring is ring0, copy the search data which will display
	// as spheres later
	if (display_sphere) {
		search_data_from_source_display.clear();
		search_data_from_source_display = search_data_from_source;
		max_priority_from_source = search_data_from_source[meeting_node_id].priority;
		min_priority_from_source = search_data_from_source[source_id].priority;

		search_data_from_goal_display.clear();
		search_data_from_goal_display = search_data_from_goal;
		max_priority_from_goal = search_data_from_goal[meeting_node_id].priority;
		min_priority_from_goal = search_data_from_goal[goal_id].priority;

		meeting_node_id_display = meeting_node_id;
		display_sphere = false;
	}
}

void MovementGraph::drawMMSphere() const
{
	for (unsigned int k = 0; k < node_array.size(); k++) {
		glPushMatrix();
		glTranslatef((GLfloat)node_array[k].position.x,
			(GLfloat)node_array[k].position.y,
			(GLfloat)node_array[k].position.z);

		// draw the starting node
		if (search_data_from_source_display[k].best_previous_node_id == k) {
			glColor3f(0.0f, 1.0f, 1.0f);
			glutSolidSphere(3.0, 50, 50);
		} // draw the destination node
		else if (k == meeting_node_id_display) {
			glColor3f(0.6f, 0.0f, 1.0f);
			glutSolidSphere(3.0, 50, 50);
		} // draw the meeting node
		else if (search_data_from_goal_display[k].best_previous_node_id == k) {
			glColor3f(1.0f, 1.0f, 1.0f);
			glutSolidSphere(3.0, 50, 50);
		} // draw all nodes reached from the forward direction
		else if (search_data_from_source_display[k].best_previous_node_id < NEVER_REACHED) {
			glColor3f(0.0f, 0.5f + 0.5f*(max_priority_from_source - search_data_from_source_display[k].priority)
				/ (max_priority_from_source - min_priority_from_source), 1.0f);
			glutSolidSphere(1.5, 50, 50);
		}// draw all nodes reached from the backward direction
		else if (search_data_from_goal_display[k].best_previous_node_id < NEVER_REACHED) {
			glColor3f(1.0f, (max_priority_from_goal - search_data_from_goal_display[k].priority)
				/ (max_priority_from_goal - min_priority_from_goal), 1.0f);
			glutSolidSphere(1.5, 50, 50);
		}
		glPopMatrix();
	}
}

//the rest of functions is all output functions
void MovementGraph::drawPath() const
{
	const float LINE_ABOVE = 1.0f;

	for (unsigned int i = 0; i < node_array.size(); i++) {
		for (unsigned int j = 0; j < node_array[i].neighbor.size(); j++) {
			unsigned int target = node_array[i].neighbor[j].node_id;
			float weight = node_array[i].neighbor[j].weight;

			glColor3f(1.0f, 1.0f - weight / 150.0f, 0.0f);
			glBegin(GL_LINE_STRIP);
			glVertex3d(node_array[i].position.x,
				node_array[i].position.y + LINE_ABOVE, node_array[i].position.z);
			glVertex3d(node_array[target].position.x,
				node_array[target].position.y + LINE_ABOVE, 
				node_array[target].position.z);
			glEnd();
		}
	}
}

void MovementGraph::drawSphere() const
{
	drawAStarSphere();
	//drawMMSphere();
}

Vector3 MovementGraph::getPosition(unsigned int node_id)
{
	return node_array[node_id].position;
}

unsigned int MovementGraph::getDiskId(unsigned int node_id)
{
	return node_array[node_id].disk_id;
}

stack<unsigned int> MovementGraph::getPath()
{
	return path;
}