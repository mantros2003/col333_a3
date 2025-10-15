## TODO
- [ ] Implement Tseitin transformation
- [ ] How to implement J constraint

## Implementation idea
Two ideas:
- Define M(i,j,k,d) as line k between (i,j) and one of its neighbours defined by d. Then we can define auxillary variables to represent line at cell, turns can be defined as (if (at cell) then (horizontal and vertical edge)).
- Define I(i,j,k) as line k at (i,j). Turns can be defined by (not I or ((U or D) and (L or R))).