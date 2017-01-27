Data Oriented Design by example

Commit [b672ca1](https://github.com/nikitablack/cpp-tests/commit/b672ca106ef5c88a23d7f8e2a909e07d6e54ca4a) have initial version of the programm. This version based on traditional OOP design. Here each object checked for collisions with all other objects. In this manner only 550 shapes can run on 30 fps (Intel Core i7).

Commit [8364d7c](https://github.com/nikitablack/cpp-tests/commit/8364d7c55c8948ab2fc5630e828d44551c6db951) added spatial grid. Now approximately 8000 shapes can be simulated at 30 fps.

Commit [8d83cb3](https://github.com/nikitablack/cpp-tests/commit/8d83cb351008a3faf3629279ebaf49ff3fc71b17) added multithreading. Now approximately 15000 shapes can be simulated at 30 fps on 4 cores.
