# Chess_AI
Chess AI for C++

Main alterable variable is maxMoves. The chess AI takes a long time to process moves when this value is too high (more than two minutes on average when above 3). 

Lag time is due to the lack of alpha-beta pruning implementation. 



Player always plays as white. 

Player has access to: castling, selective promotions, en-passent. AI (black) will always promote to queen. Does not view castling or en-passent as possible moves. 

Created using mini-max algorithm as its basis. 

AI displays in both french and english. 
