
Maze solver

Give a maze where white space is walkable terrain, ^ the starting position and E the exit.

Improvement ideas:
-Fast check:
    We could check all the entrances first, look for the minimun distance aka straight from start to exit, and determine from there should we even try the options with less steps
-Find the optimal route:
    The algorithm now just finds the first route that satisfies the step limit
    Maybe just lowering the step limit after we have found a route and then running the algorithm again with that limit? Would be far from optimal solution
-Optimize the code
    Damn it takes a long time if there is a lot of open space