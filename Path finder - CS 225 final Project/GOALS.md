# CS225 Data Structures

## Team Contract (minwooc2 - dka3 - zhenyuw5 - dl14)

## Communication

- **Team Meetings** - We will meet weekly on Thursdays from 12-2PM using Zoom. One of us will take notes on a Google Doc for the meeting. This role will be rotated through each member. We will also keep another informal Google Doc as a place to briefly update what we worked on.

- **Assistance** - We will communicate using email and Groupme.

- **Respect** - We commit to devoting a similar amount of time per person to speak at each meeting where they can express their opinions and ideas without judgement. Should a disagreement arise we will vote for a simple majority. We will all try our best to not be late to meetings and respond to messages in a timely manner (except for personal emergencies). We also understand that everyone has other classes and responsibilities and some weeks may be a “slow week” and deal with this when it comes up on a case by case basis. 

## Collaboration

- **Work Distribution** - During our weekly meetings, we are planning to divide the larger task at hand into smaller chunks that can be completed individually by each of the group members. Each of us will volunteer to take a task. If any one of us feels as if the task at hand is too large, we will bring it up to the entire group and redelegate tasks if necessary.

- **Time Commitment** - As a tentative time commitment, we promise to spend at least 2 hours and at most 4 hours on our tasks for the week.

- **Conflict Resolution** - All conflicts within the team will be decided with a vote by all participating team members. If possible, we will make the vote anonymous to suspend judgement as much as possible.

## Leading Question

- Our goal is to link two airports on Openflights’ provided routes dataset using the ‘shortest possible path’, where a path is a flight between two cities. Using this, we plan to show the extent as to how interconnected the world is. From this, the question we will ask is ‘how can we show the shortest path between any two cities with airports?’<br><br>A successful project would show a connection between two cities (as inputs) using a list of airports ‘between’ those two airports and the total distance between them (as outputs).

## Dataset Acquisition and Processing

- For this project, we will be using the OpenFlights dataset. GIven that the data is standardized and identical to whoever accesses the dataset, we will download, store, and process the data directly from the OpenFlights website onto our individual machines.<br><br>Potential errors such as missing entries won’t pose much of a problem to us, since the dataset already  took care of missing values by inserting NULL values in its place. If a datapoint in a path is found to be faulty, we can nullify the path currently being made and choose a different path.

## Graph Algorithms

- We will use Depth-First-Search (DFS) to traverse through the graph, and we will use Dijkstra’s algorithm and Iterative deepening depth-first-search (IDDFS) to find the shortest path. Depth First Search needs a connected graph as the input while the Dijkstra’s and IDDFS requires a connected graph and a beginning and end node in the tree as the inputs. The path finding algorithm will output the shortest path and its distance. The DFS will output a path to every node in the graph. Djikstra’s algorithm has a time complexity of **O(E\*log(N))** where N is the number of nodes in the graph and E is the number of edges in the graph. The time complexity of IDDFS (in a well balanced tree) is the same as the time complexity of a Breadth-First-Search. That is, the time complexity will be **O(log(E + N))** where N is the number of nodes in the graph and E is the number of edges in the graph. The DFS has the same time complexity as the IDDFS. 

## Timeline

- Tasks:
  - Download Data and push it to github (April 11th)
  - Data processing (April 15th)
  - Depth First Search algorithm implementation (Mid-Project Checkin)
  - IDDFS algorithm implementation (April 29th)
  - Dijkstra’s algorithm implementation (May 6th)
  - Build test suites (May 6th)
  - Make presentation slides (May 11th)
  - Make presentation video (May 11th)


Signed by: Minwoo Cho <br>
Signed by: Daniel Lopez <br>
Signed by: Zhenyu Wang <br>
Signed by: Daniel Ahn
