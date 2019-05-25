A small, easy to use Entity-Component-System (ECS).

---

# Warning
At the moment I'm just experimenting with making an API I'll enjoy using. This is not optimized and probably full of bugs.

# Example Code

Create/destroy an entity:

```cpp
#include "ecs/Manager.h"

ecs::Manager manager;
ecs::Entity my_entity = manager.Create();

// Sometime later, perhaps at the end of the game session:
manager.Destroy(my_entity);
```

Assign a position component:

```cpp
// Declared somewhere...
struct Position {
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;

  Position() = default;
  Position(float x, float y, float z) : x(x), y(y), z(z) {}
};

// ...

manager.Assign<Position>(my_entity, 25.0f, 50.0f, 100.0f);

```

Retrieve a component by reference:

```cpp
Position& pos = manager.Get<Position>(my_entity);
```

Iterate all entities with a specific component:

```cpp
manager.ForEach<Position>([](Entity entity, auto& pos) {
  // Do something with 'pos'...
});

```

Iterate all entities with a collection of components:

```cpp
manager.ForEach<Position, Mesh>([](Entity entity, auto& pos, auto& mesh) {
  // Draw 'mesh' at 'position'...
});
```

# Inspirations

This is inspired by many talks on ECS, some implementations I've used in the past and some online, including:

- [Overwatch's ECS talk](https://www.youtube.com/watch?v=W3aieHjyNvw)
- [Mike Acton's various talks on Data Oriented Design](https://www.youtube.com/watch?v=rX0ItVEVjHc)
- [EnTT](https://github.com/skypjack/entt)
- [Our Machinery](https://ourmachinery.com/post/making-the-move-rotate-scale-gizmos-work-with-any-component/)
- [Building a Data-Oriented Entity System](http://bitsquid.blogspot.com/2014/08/building-data-oriented-entity-system.html)
- [Molecular Matters: Adventures in Data-Oriented Design](https://blog.molecular-matters.com/2011/11/03/adventures-in-data-oriented-design-part-1-mesh-data-3/)
