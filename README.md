
Maze solver
===========
Give a maze where white space is walkable terrain, ^ the starting position and E the exit.

Improvement ideas
=================
1. Optimize
- The algorithm isn't good nor optimized right now
- If there is a lot of white space, the algortihm can take a long time
- Would propably do a flood-fill
2. Finding the optimal route
- The algorithm now finds a route and then tries to optimize it by giving the algorithm less steps to work with
- This is very unefficient because we just throw all the earlier computation into trash and always start from the beginning and it might take ridiculous amount of time
3. Passing arguments
- What do we really need? Most of the data could be derived from the maze map, so should we just always check from that or check once and pass it around
- Check all values for example in case of const
4. Error handling
- There is always room for more error handling
- More time testing and iterating
