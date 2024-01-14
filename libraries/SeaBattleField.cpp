#include "SeaBattleField.h"
#include <algorithm>

#include <fstream>

SeaBattleField :: SeaBattleField(uint32_t size, Visibility visibility) : field_(size, std::vector<Cell>(size, Cell::Empty)),
                                                  user_field_(size, std::vector<Cell>(size, Cell::Unknown)),
                                                  size_(size),
                                                  visibility_(visibility)
                                                  {}

SeaBattleField :: SeaBattleField(uint32_t size) : SeaBattleField(size, Visibility::Fog) {}

SeaBattleField :: SeaBattleField() : SeaBattleField(10) {}

bool SeaBattleField :: is_on_field(const std::pair<uint32_t, uint32_t> &coordinates) const {
    return (0 <= coordinates.first && coordinates.first < size_ &&
            0 <= coordinates.second && coordinates.second < size_);
}

bool SeaBattleField :: is_empty(const std::pair<uint32_t, uint32_t> &coordinates) const {
    return (is_on_field(coordinates) &&
            field_[coordinates.first][coordinates.second] == Cell::Empty);
}

bool SeaBattleField :: add_ship(const std::pair<uint32_t, uint32_t> &coordinates,
                                Orientation orientation,
                                uint32_t length) 
{


    if(length <= 0 || length >= 5) return false;
    auto curr_coordinates(coordinates);

    auto new_field(field_);

    Ship ship;

    for(size_t df = 0; df < length; df++) {
        if(is_on_field(curr_coordinates) && is_empty(curr_coordinates)) {
            new_field[curr_coordinates.first][curr_coordinates.second] = Cell::Ocuppied;
            ship.coordinates_.push_back(curr_coordinates);
            ship.is_flooded.push_back(false);
            if(orientation == Orientation::Horizontal) curr_coordinates.second++;
            else curr_coordinates.first++;
        } else return false;
    }

    auto start_coord(coordinates);
    std::pair<uint32_t, uint32_t> finish_coord;
    if(orientation == Orientation::Vertical){
        finish_coord.first = start_coord.first + length - 1;
        finish_coord.second = start_coord.second;
    } else {
        finish_coord.second = start_coord.second + length - 1;
        finish_coord.first = start_coord.first;
    }

    uint32_t x1, y1, x2, y2;
    x1 = std::max(static_cast<int32_t>(start_coord.second) - 1, static_cast<int32_t>(0));
    x2 = std::min(static_cast<int32_t>(finish_coord.second) + 1, static_cast<int32_t>(size_ - 1));
    y1 = std::max(static_cast<int32_t>(start_coord.first) - 1, static_cast<int32_t>(0));
    y2 = std::min(static_cast<int32_t>(finish_coord.first) + 1, static_cast<int32_t>(size_ - 1));

    for(uint32_t i = x1; i < x2 + 1; i++){
        for(uint32_t j = y1; j < y2 + 1; j++){
            if(field_[j][i] == Cell::Ocuppied || field_[j][i] == Cell::Flooded){
                return false;
            }
        }
    }

    ships_.push_back(ship);
    field_ = new_field;
    return true;
}

std::string SeaBattleField :: hit_at(const std::pair<uint32_t, uint32_t> &coordinates) {
    bool result = false;
    if(is_on_field(coordinates)) {
        for(auto &ship : ships_) {
            auto it = std::find(ship.coordinates_.begin(), ship.coordinates_.end(), coordinates);
            if(it != ship.coordinates_.end()) {
                auto dist = std::distance(ship.coordinates_.begin(), it);
                if(ship.is_flooded.at(dist)) {
                    return "Shot at the sunken part of the ship";
                    result = true;
                } else {
                    result = true;
                    ship.is_flooded.at(dist) = true;
                    field_[coordinates.first][coordinates.second] = Cell::Flooded;
                    user_field_[coordinates.first][coordinates.second] = Cell::Flooded;
                }
                uint32_t cnt = 0;
                for(auto value : ship.is_flooded) {
                    if(value == true) cnt++;
                }
                if(cnt == ship.is_flooded.size()) {
                    return "Ship is flooded";
                }
            }
        }
    } else return "Invalid coordinates";
    
    user_field_[coordinates.first][coordinates.second] = result ? Cell::Flooded : Cell::Empty;
    return result ? "Hit" : "Miss";
}

std::ostream& operator<<(std::ostream& out, const SeaBattleField &sea) {
    out << "   ";
    for(size_t i = 0; i < sea.size_; i++) out << " " << static_cast<char>('A' + i) << " ";
    out << std::endl;
    for(size_t i = 0; i < sea.size_; i++) {
        out << " " << i + 1 << (i + 1 < 10 ? " " : "");
        for(size_t j = 0; j < sea.size_; j++) {
            Cell cell = Cell::Unknown;
            if(sea.visibility_ == Visibility::Clear) {
                cell = sea.field_[i][j];
            } else {
                cell = sea.user_field_[i][j];
            }
            switch(cell) {
                case Cell::Unknown:
                    out << "[?]";
                    break;
                case Cell::Empty:
                    out << "[~]";
                    break;
                case Cell::Flooded:
                    out << "[*]";
                    break;
                case Cell::Ocuppied:
                    out << "[#]";
                    break;
            }
        }
        out << '\n';
    }
    return out << '\n';
}

void SeaBattleField :: set_visible(Visibility visible) {
    visibility_ = visible;
}

void SeaBattleField :: start_game() {
    std::cout << "Введите что вы хотите сделать: \n" 
              << "(1) - добавить корабль : (координаты (например E3), ориентация(1 - горизонтальная, 0 - вертикальная), длина корабля - число)\n"
              << "(2) - выстрелить : (координаты (например D8))\n"
              << "(3) - закончить игру\n"
              << "(4) - изменить видимость поля(0 - туман войны, 1 - без тумана)\n";
    std::cout << *this;

    std::string choice;

    while(true) {
        std::cin >> choice;
        if(choice == "1") {
            std::string str_coords;
            bool orientation;
            uint32_t length;
            std::cin >> str_coords >> orientation >> length;

            std::pair<uint32_t, uint32_t> coordinates;
            coordinates.second = (str_coords[0] - 'A');
            coordinates.first = std::stoi(str_coords.substr(1, str_coords.size() - 1)) - 1;
            
            auto result = add_ship(coordinates,
                                    orientation ? Orientation::Horizontal : Orientation::Vertical,
                                    length);

            if(result) {
                std::cout << "Корабль успешно установлен\n";
            } else {
                std::cout << "Корабль не удалось установить\n";
            }
           
            std::cout << *this;
        } else if(choice == "2") {
            std::string str_coords;
            std::cin >> str_coords;
            std::pair<uint32_t, uint32_t> coordinates;
            coordinates.second = (str_coords[0] - 'A');
            coordinates.first = std::stoi(str_coords.substr(1, str_coords.size() - 1)) - 1;
            
            std::cout << hit_at(coordinates) << '\n' << *this;
        } else if(choice == "3") {
            return;
        } else if(choice == "4") {
            bool visible;
            std::cin >> visible;
            set_visible(visible ? Visibility::Clear : Visibility::Fog);
            std::cout << *this;
        }
        else {
            std::cout << "Неверный код команды\n";
        }
    }
}