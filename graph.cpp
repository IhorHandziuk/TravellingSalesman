#include <cmath>
#include "graph.h"

Graph::Graph(int ver_num)
{
	this->create(ver_num, 100);
}

Graph::~Graph(void)
{
	for (int i = 0; i < graph.size(); i++) delete graph[i];
	graph.clear();
	routes.clear();
}

void Graph::create(int ver_num, int link_prob = 100)
{
	for (int i = 0; i < graph.size(); i++) delete graph[i];
	graph.clear();
	routes.clear();
	{
		srand(time(NULL));
		for (int i = 0; i < ver_num; i++)
		{
			Vertex *curr = new Vertex;
			curr->x=rand()%(gl_width-1)+1;
			curr->y=rand()%(gl_height-1)+1;
			graph.push_back(curr);
		}
		for (int i=0;i<graph.size();i++)
		{
			for (int j=i+1;j<graph.size();j++)
			{
				if (rand()%100<link_prob)
				{
					graph[i]->pts.push_back(graph[j]);
					graph[j]->pts.push_back(graph[i]);
				}
			}
		}
	}
}	

void Graph::read(FILE* mat, FILE* coor)
{
	for (int i = 0; i < graph.size(); i++) delete graph[i];
	graph.clear();
	const int NMAX = 500;
	int vertice_number = 0;
	bool matrix[NMAX][NMAX];
	char buff[20];
	int u = 0,v = 0;
	while (fgets (buff, sizeof(buff), mat) > 0) vertice_number++;
	rewind(mat);
	while (fgets (buff, sizeof(buff), mat) > 0) {
		for (; v < vertice_number; v++) {
			matrix[u][v] = buff[v*2] - '0';
			if (v == vertice_number-1) u++;
		}
		v = 0;
	}
	char b_x[10];
	char b_y[10];
	float coordinates[NMAX][2];
	for (int q = 0; fscanf (coor, "%s %s", b_x, b_y) == 2; q++)
	{
		coordinates[q][0] = atof(b_x);
		coordinates[q][1] = atof(b_y);
	}
	for (int w = 0; w < vertice_number; w++) 
	{
		Vertex* cur_ver = new Vertex;
		cur_ver->x = coordinates[w][0];
		cur_ver->y = coordinates[w][1];
		graph.push_back(cur_ver);
	}
	for (int s=0;s<graph.size();s++)
	{
		for (int d=s+1;d<graph.size();d++)
		{
			if (matrix[s][d] == true)
			{
				graph[s]->pts.push_back(graph[d]);
				graph[d]->pts.push_back(graph[s]);
			}
		}
	}
}

float dist (Vertex* a, Vertex* b)
{
	return sqrt(pow((a->x - b->x),2)+pow((a->y - b->y),2));
}

float Graph::cycle_length (vector<int> route)
{
	if (route.size() != graph.size()) return 0;
	float len = 0;
	for (int it = 0; it < route.size()-1; it++)
		len += dist(graph[route[it]], graph[route[it+1]]);
	len += dist(graph[route[route.size()-1]], graph[route[0]]);
	return len;
}

float Graph::length (vector<int> route)
{
	if (route.size() < 2) return 0;
	float len = 0;
	for (int it = 0; it < route.size()-1; it++)
		len += dist(graph[route[it]], graph[route[it+1]]);
	return len;
}

void Graph::make_first_gen(int size)
{
	int* route = new int[graph.size()];
	for (int i = 0; i < graph.size(); i++) route[i] = i;
	int time_limit = 100000;
	while (size != routes.size() && time_limit--)
	{	
		srand(time(NULL));
		for (int p = 1; p < graph.size(); p++)
			swap(route[p], route[rand()%(graph.size()-1)+1]);
		vector<int> curr;
		float len;
		for (int i = 0; i < graph.size(); i++)
			curr.push_back(route[i]);
		len = cycle_length(curr);
		routes.insert(pair<float, vector<int>>(len, curr));
	}
}

int curr_next(vector<int> curr, int res_at)
{
	for (int i = 0; i < curr.size(); i++)
	{
		if (curr[i] == res_at) 
			return curr[(i+1 >= curr.size())?i+1-curr.size():i+1];
	}
}

vector<int> posterity(vector<int> curr_i, vector<int> curr_j)
{
	unordered_set<int> res_set;
	vector<int> res;
	res_set.insert(curr_i[0]);
	int i = 1;
	int res_set_at_i_minus_one = curr_i[0];
	int time_limit = 1000;
	while (res_set.size() != curr_i.size() && time_limit--)
	{
		if (i % 2) {
			res_set.insert(curr_next(curr_i, res_set_at_i_minus_one));
			if (res_set.size()-1 != i) {
				res_set.insert(curr_next(curr_j, res_set_at_i_minus_one));
				res_set_at_i_minus_one = curr_next(curr_j, res_set_at_i_minus_one);
			}
			else res_set_at_i_minus_one = curr_next(curr_i, res_set_at_i_minus_one);
		}
		else {
			res_set.insert(curr_next(curr_j, res_set_at_i_minus_one));
			if (res_set.size()-1 != i) {
				res_set.insert(curr_next(curr_i, res_set_at_i_minus_one));
				res_set_at_i_minus_one = curr_next(curr_i, res_set_at_i_minus_one);
			}
			else res_set_at_i_minus_one = curr_next(curr_j, res_set_at_i_minus_one);
		}
		i++;
	}
	for (auto i = res_set.begin(); i != res_set.end(); i++)
		res.push_back(*i);
	return res;
}

void Graph::mutate(map <float, vector<int>>& routes, map <float, vector<int>>::reverse_iterator bad_one)
{
	srand(time(NULL));
	vector<int> curr = bad_one->second;
	routes.erase(next(bad_one).base());
	int rand_vertex_index = rand() % curr.size();
	int ver = curr.at(rand_vertex_index);
	curr.erase(curr.begin() + rand_vertex_index);
	curr.insert(curr.begin()+rand()%curr.size(), ver);
	float len = cycle_length(curr);
	routes.insert(pair<float, vector<int>>(len, curr));
}

void Graph::next_gen()
{
	map <float, vector<int>> new_routes;
	vector<int> curr_i, curr_j, son;
	for (auto i = routes.begin(); i != routes.end(); i++)
	{
		curr_i = i->second;
		for (auto j = routes.begin(); j != routes.end(); j++)
		{
			if (i == j) continue;
			curr_j = j->second;
			son = posterity(curr_i, curr_j);
			float len = cycle_length(son);
			if (len > 0) new_routes.insert(pair<float, vector<int>>(len, son));
		}
	}
	int mutate_ones = new_routes.size()*3/4;
	int count = 0;
	for (auto l = new_routes.rbegin(); l != new_routes.rend(); l++)
	{
		count++;
		this->mutate(new_routes, l);
		if (count == mutate_ones) break;
	}
	new_routes.insert(*routes.begin());
	while (new_routes.size() > routes.size())
		new_routes.erase(prev(new_routes.end()));
	routes = new_routes;
}

//OpenGL visualization module
/*********************************************/
void Graph::draw()
{
	glLineWidth(0.6);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.1, 0.1, 0.1);
	glPointSize(5.0);
	glBegin(GL_POINTS);
	for (int i=0;i<graph.size();i++)
		glVertex2d(graph[i]->x,graph[i]->y);
	glEnd();
	
	glColor3f(0.5, 0.5, 1.0);
	glBegin(GL_LINES);

	for (int i=0;i<graph.size();i++)
	{
		for (int j=0;j<graph[i]->pts.size();j++)
		{
			float begx=graph[i]->x;
			float begy=graph[i]->y;
			float endx=graph[i]->pts[j]->x;
			float endy=graph[i]->pts[j]->y;
			
			glVertex2d(begx,begy);
			glVertex2d(endx,endy);
		}
	}
	glEnd();

	glColor3f(1.0, 0.0, 0.0);
	glLineWidth(1.4);
	glBegin(GL_LINE_LOOP);
	if (!routes.empty()) {
		for (int i = 0; i < routes.begin()->second.size(); i++) {
			glVertex2d(graph[routes.begin()->second[i]]->x, graph[routes.begin()->second[i]]->y);
		}
	}
	glEnd();
	glFlush();
	glutSwapBuffers();
}

void reshape(int w,int h)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluOrtho2D(0,gl_width,gl_height,0);
	glViewport(0,0,w,h);
}

/*******************************************
This great perversion had been done
because of glutDisplayFunk and glutKeabordFunk
don't take functions that are member of the class
*********************************************/
Graph* g_CurrentInstance;

void drawCallback()
{
	g_CurrentInstance->draw();
}

void keypress(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'n':
		g_CurrentInstance->create(rand()%5+4, 100);
		g_CurrentInstance->make_first_gen(100);
		break;
	case 'g':
		g_CurrentInstance->next_gen();
		break;
	case 's':
		glutIdleFunc(nullptr);
		break;
	};
	glutPostRedisplay();
}

void nextGenCallback()
{
	g_CurrentInstance->next_gen();
	glutPostRedisplay();
}

void Graph::setupDrawCallback()
{
	g_CurrentInstance = this;
	glutDisplayFunc(drawCallback);
}

void Graph::setupKeypressCallback()
{
	g_CurrentInstance = this;
	glutKeyboardFunc(keypress);
}

void Graph::setupNextGen()
{
	g_CurrentInstance = this;
	glutIdleFunc(nextGenCallback);
}
/*********************************************/

void Graph::display_start()
{
	int argc = 0;
	char* argv = "";
	glutInit(&argc, &argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 400);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("Graph application");
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	setupDrawCallback();
	setupKeypressCallback();
	setupNextGen();
	glutReshapeFunc(reshape);
}
/*********************************************/
void Graph::display_loop()
{
	glutMainLoop();
}

