/*
This assignment is done by :

1) Motasem Amoura 139552
2) Mohammad Ammourah 140798

*/

#include <bits/stdc++.h>
using namespace std;
struct process
{
	int ArrivalTime;
	int ProcessingTime;
	string name;
	int turnAround;
	int response;
	int delay;
	int Pid;

	bool operator>(const process &e) const // overload the > operator for pq
	{
		if ((*this).ArrivalTime == e.ArrivalTime)
			return (*this).ProcessingTime > e.ProcessingTime;
		return (*this).ArrivalTime > e.ArrivalTime;
	}
	bool operator<(const process &e) const
	{
		return (*this).Pid > e.Pid;
	}
};

int main()
{

	// if you want the program to read and write from the terminal ; just comment the following two lines 
	ifstream cin("in.txt");
	ofstream cout("out.txt");

	int N;
	cin >> N;

	// priority queue to store the processes based on the arrival time
	priority_queue<process, vector<process>, greater<process>> pq;

	// read the file and store processes in the pq
	process p;
	for (int i = 0; i < N; i++)
	{
		cin >> p.name;
		cin >> p.ArrivalTime;
		cin >> p.ProcessingTime;
		p.Pid = i;
		pq.push(p);
	}

	stack<process> s;
	priority_queue<process, vector<process>, less<process>> finished;
	string ExcecutionOrder = "";

	// helper variables
	process temp;
	process t;

	for (int time = 0; !(s.empty() && pq.empty()); time++)
	{
		temp = pq.top();
		if (temp.ArrivalTime == time && !pq.empty())
		{
			s.push(temp);
			pq.pop();
		}

		if (!s.empty())
		{
			temp = s.top();
			ExcecutionOrder += temp.name;
			temp.turnAround = time + temp.ProcessingTime - temp.ArrivalTime;
			temp.delay = temp.turnAround - temp.ProcessingTime;
			temp.response = time - temp.ArrivalTime;
			finished.push(temp);

			s.pop();

			while (!pq.empty())
			{
				t = pq.top();
				if (t.ArrivalTime >= time && t.ArrivalTime <= time + temp.ProcessingTime)
				{
					s.push(t);
					pq.pop();
				}
				else
					break;
			}
			time += temp.ProcessingTime - 1;
		}
	}

	// print the result 
	cout << ExcecutionOrder << endl;
	while (!finished.empty())
	{
		temp = finished.top();
		cout << temp.name << ": (response="
			 << temp.response
			 << ", turnaround=" << temp.turnAround << ", delay=" << temp.delay << ")" << endl;
		finished.pop();
	}

	return 0;
}
