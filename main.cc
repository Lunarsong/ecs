#include <iostream>
#include <string>

#include "ecs/Entity.h"
#include "ecs/Manager.h"

using ecs::Entity;

struct Position {
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;

  Position() = default;
  Position(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct Name {
  std::string name;

  Name(std::string name) : name(std::move(name)) {}
};

void main() {
  ecs::Manager manager;

  Entity e0 = manager.Create();
  Entity e1 = manager.Create();
  Entity e3 = manager.Create();

  manager.Assign<Name>(e0, "Test");
  manager.Assign<Name>(e1, "Other Test");
  manager.Assign<Name>(e3, "Only Name");

  manager.Assign<Position>(e0, 25.0f, 50.0f, 100.0f);
  manager.Assign<Position>(e1, 2.0f, 8.0f, 16.0f);

  std::cout << "e0: " << manager.Get<Name>(e0).name << ", "
            << manager.Get<Position>(e0).x << ", "
            << manager.Get<Position>(e0).y << ", "
            << manager.Get<Position>(e0).z << "\n";

  std::cout << "e1: " << manager.Get<Name>(e1).name << ", "
            << manager.Get<Position>(e1).x << ", "
            << manager.Get<Position>(e1).y << ", "
            << manager.Get<Position>(e1).z << "\n";

  std::cout << "\n";

  // ForEach:
  std::cout << "ForEach: \n";
  manager.ForEach<Name>([](Entity entity, auto& name) {
    std::cout << entity.Id() << ": " << name.name << "\n";
  });
  std::cout << "\n";

  std::cout << "ForEach (Multiple): \n";
  manager.ForEach<Name, Position>(
      [](Entity entity, auto& name, auto& position) {
        std::cout << entity.Id() << ": " << name.name << ", " << position.x
                  << ", " << position.y << ", " << position.z << "\n";
      });
  std::cout << "\n";
}