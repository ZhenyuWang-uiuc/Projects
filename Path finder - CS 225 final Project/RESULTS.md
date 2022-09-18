# Results

## Dijkstra & IDDFS Algorithm

Here is the screenshot of our code:

![alt text](https://github-dev.cs.illinois.edu/cs225-sp21/minwooc2-zhenyuw5-dl14-dka3/blob/master/Example_output_all.jpg?raw=true)

Our IDDFS algorithm outputs every possible path between the two cities limited by a certain number of layovers. This allows the user to see all available flights and make a decision. We took advantage of the fact that IDDFS iterates with difference depth bounds and we output the path found on each depth bound because this would have a certain number of layovers. We also output the shortest path and its distance to help the user make a choice and to align with our project goals.

Our original goal was to find the shortest flight path between any two cities. We found that by 2014, all the major cities are connected with several direct flights and even smaller cities have  many flights to major airports. This meant that most cities were connected by at most two layovers and the total flight distance between most two cities was not far off from a theoretical direct route. We know our IDDFS and Dijkstra's algorithm works because given any two major cities, it outputs a direct flight and given two smaller airports, it almost never outputs a path with more than two intermediate cities. And when we use a third party calculator to find the distance between two cities, our algorithms output a distance that is usually within 5% of that distance (the minor difference comes from actual position of the airport in the cities and our program don't consider the altitude as part of the distance calculation). Furthermore, our IDDFS and Dijkstra's algorithm used two very different methods but their outputs were the same so we know that these functions work as intended. 

## Interesting Discoveries 

With no direct flights from BOS (Boston) to ICN (Incheon), people tend to layover at JFK (New York City) or LAX (Los Angeles) for their flights. But using our program, we found that the shortest path from BOS to ICN is actually through YYZ (Toronto). We started the project by assuming the price of flight tickets are directly proportional to the total distance traveled. However, when we looked up the actual ticket price and compared to the distances, we found that ticket prices don’t always represent the total flight distance, rather, we predict there must be other factors that airlines consider to create prices.   

We started the project by assuming the price of flight tickets are directly proportional to the total distance traveled. However, when we looked up the actual ticket price and compared to the distances, we found that ticket prices don’t always represent the total flight distance, rather, we predict there must be other factors that airlines consider to create prices. 

## Step into Dijkstra & IDDFS Algorithm

How do we know our answer is correct? We create a small graph to make sure Dijkstra & IDDFS Algorithm exactly find the path that we are looking for.

<img src="https://github-dev.cs.illinois.edu/cs225-sp21/minwooc2-zhenyuw5-dl14-dka3/blob/master/graph%20with%20path.png" alt="drawing" width="400"/><img src="https://github-dev.cs.illinois.edu/cs225-sp21/minwooc2-zhenyuw5-dl14-dka3/blob/master/graph%20with%20path%20(have%20distance).png" alt="drawing" width="400"/>

For instance, if we want to find the shortest path between "A" and "H". Our desired path is through `"A" -> "C" -> "E" -> "H"`. We build a test case to show whether this one did what we want. 

The following is the screenshot of that test case:

<img src="https://github-dev.cs.illinois.edu/cs225-sp21/minwooc2-zhenyuw5-dl14-dka3/blob/master/test%20case.png" alt="drawing" width="600"/>

The result is:

<img src="https://github-dev.cs.illinois.edu/cs225-sp21/minwooc2-zhenyuw5-dl14-dka3/blob/master/result.png" alt="drawing" width="600"/>

This proves that Dijkstra Algorithm works exactly as our desire. As we mention above, IDDFS has the same output as the Dijkstra Algorithm. This also proves that Dijkstra Algorithm also works as well.
