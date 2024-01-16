#ifndef quas_exort
#define quas_exort

#include <iostream>
#include <vector>
#include <string>

enum Cell {Unknown, Empty, Flooded, Ocuppied};
enum Orientation{Vertical, Horizontal};
enum Visibility{Fog, Clear};

struct Ship {
    std::vector<std::pair<uint32_t, uint32_t>> coordinates_;
    std::vector<bool> is_flooded;
};

class SeaBattleField {
private:
    std::vector<std::vector<Cell>> field_;      // поле игры
    uint32_t size_;
    Visibility visibility_;

    std::vector<Ship> ships_; // корабль - вектор координат, из клеток которых он состоит

    void clear_field(Cell** field);
    bool is_on_field(const std::pair<uint32_t, uint32_t> &coordinates) const;
public:
    explicit SeaBattleField(uint32_t size);
    explicit SeaBattleField(uint32_t size, Visibility visibility);
    SeaBattleField();

    bool add_ship(const std::pair<uint32_t, uint32_t> &coordinates,
                  Orientation orientation,
                  uint32_t length);

    void hit_at(const std::pair<uint32_t, uint32_t> &coordinates);

    void set_visible(Visibility visibility);

    friend std::ostream& operator<<(std::ostream& out, const SeaBattleField &sea);

    void start_game();
};

#endif