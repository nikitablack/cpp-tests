Data Oriented Design by example

Commit [8363e60](https://github.com/nikitablack/cpp-tests/commit/8363e60c614377cccb777625805b6aea9e699117) have initial version of the programm. This version based on traditional OOP design. Here each object checked for collisions with all other objects. In this manner only 550 shapes can run on 30 fps (Intel Core i7).

Commit [f726cec](https://github.com/nikitablack/cpp-tests/commit/f726cec7591a2568ef41fbe53623e5447bfdde45) added spatial grid. Now approximately 9500 shapes can be simulated at 30 fps.

Commit [ba57d29](https://github.com/nikitablack/cpp-tests/commit/ba57d299a26545d6045bd84797bc69d98ceca7bd) added multithreading. Now approximately 15000 shapes can be simulated at 30 fps on 4 cores.
