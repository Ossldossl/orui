# omui
retained-mode ui-library in c using OpenGL

## Layout
* Flexbox-like layout system
* pref_width, pref_height represent the starting width/height of an element
* from there on, if grow_factor > 0, the element grows until either there is no space left or max_height is reached, whatever comes first.
* grow-/shrink_factor specify how much the element is going to grow/shrink relative to it's siblings. 
For example a grow_factor of 2 specify that the current element gets twice as much of the free space (or grows twice as much) as it's siblings.
 
## Styling
* Each widget has a number of states (like HOVERED, PRESSED, ACTIVE, INACTIVE)
* you can associate properties with each state
* from the main state (in this case ACTIVE), only deltas of the changing properties are stored, and those only going from ACTIVE STATE->NEW STATE
* deltas = deltas(state to active) + deltas(active to new state), where overlapping property-deltas are discarded. In this case only the deltas going from the current state to the new state will be used.
* Every element has:
   1. an integer representing the current state it is in
   2. an array of transition structs (the deltas) (~ transition_t[STATE_COUNT])
* 