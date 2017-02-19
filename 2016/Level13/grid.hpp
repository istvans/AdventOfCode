#pragma once

#include <pdcurses/curses.h>

#include <boost/variant.hpp>

#include <atomic>
#include <mutex>
#include <queue>
#include <tuple>
#include <thread>
#include <utility>
#include <unordered_map>

#ifndef KEY_ESC
# define KEY_ESC 0x1b
#endif
#ifndef KEY_SPACE
# define KEY_SPACE 0x20
#endif
#ifndef KEY_CARRIAGE_RETURN
# define KEY_CARRIAGE_RETURN 0xd
#endif
#ifndef KEY_NEW_LINE
# define KEY_NEW_LINE 0xa
#endif
#ifndef TRUE_WHITE
# define TRUE_WHITE 15
#endif

#define YES_KEY         'y'
#define NO_KEY          'n'

namespace Display
{

template <class Coor, class ObjectKey, class InfoKey,
    class ObjHash = std::hash<ObjectKey>,
    class InfoHash = std::hash<InfoKey>>
class Grid
{
public:
    using position_t = std::pair<Coor, Coor>;
    using object_map_t = std::unordered_map<ObjectKey, char, ObjHash>;
    using info_setup_t = std::vector<std::tuple<InfoKey, std::string
        , std::string>>;
    using curses_coor_t = int;
    using time_t = std::chrono::microseconds;

private:
    using time_num_t = time_t::rep;
    using info_vec_t = std::vector<InfoKey>;
    using info_map_t = std::unordered_map<InfoKey, std::string, InfoHash>;
    using info_pos_t = std::unordered_map<InfoKey, position_t, InfoHash>;

    enum class Kind {Info, Obj};

    struct ObjPack
    {
        ObjectKey       _key;
        position_t      _pos;
    };

    struct InfoPack
    {
        InfoKey         _key;
        std::string     _info;
    };

    using StaffData =  boost::variant<ObjPack
        ,InfoPack>;

    struct Staff
    {
        Kind                _kind;
        StaffData           _data;

        template <class Variant>
        Staff(Kind kind_, Variant&& data_)
            : _kind{kind_}
            , _data{std::move(data_)}
        {}
    };

    object_map_t                                    _objects;
    info_vec_t                                      _info_order;
    info_map_t                                      _info_labels;
    info_map_t                                      _info_data;
    
    bool                                            _color_mode;
    bool                                        _quit_confirmation_needed;

    WINDOW*                                         _info_bar;
    WINDOW*                                         _grid;
    std::string                                     _sys_info;

    time_num_t                                      _delay;
    time_num_t                                      _delay_change;

    size_t                                          _init_steps;

    const char                                      _info_separator;

    std::thread                                     _keyboard_thread;
    std::thread                                     _drawer_thread;
    std::mutex                                      _drawer_mutex;
    std::atomic_bool                                _grid_run;
    std::atomic_bool                                _paused;
    std::atomic_bool                                _step;
    std::queue<Staff>                               _drawer_queue;

    std::mutex                                      _display_mutex;

    WINDOW* new_window(curses_coor_t height_, curses_coor_t width_
        , curses_coor_t starty_, curses_coor_t startx_)
    {
        WINDOW* local_win = NULL;
        local_win = newwin(height_, width_, starty_, startx_);
        if (NULL == local_win) {
            throw std::runtime_error{"Failed to initialize window!"};
        }
        ::wrefresh(local_win);
        return local_win;
    }

    inline curses_coor_t c2cc(const Coor c_) const
    {
        return static_cast<curses_coor_t>(c_);
    }

    void handle_keyboard()
    {
        _keyboard_thread = std::thread{[this]() {
            while(_grid_run) {
                auto ch = ::getch();
                switch (ch) {
                    case 'p':
                    {
                        _paused = !_paused;
                        print_info();
                        break;
                    }
                    case KEY_SPACE:
                    {
                        bool expected_step = false;
                        _step.compare_exchange_weak(expected_step, true);
                        break;
                    }
                    case KEY_RESIZE:
                    {
                        {
                        std::unique_lock<std::mutex> ul{_display_mutex};
                        ::endwin();
                        ::refresh();
                        //::clear();
                        }
                        sys_info_update("resize");
                        break;
                    }
                    case KEY_UP:
                    {
                        if (_delay > _delay_change) {
                            _delay -= _delay_change;
                        } else {
                            _delay = 0;
                        }
                        break;
                    }
                    case KEY_DOWN:
                    {
                        _delay += _delay_change;
                        break;
                    }
                    case KEY_CARRIAGE_RETURN:
                    case KEY_NEW_LINE:
                    case YES_KEY:
                    {
                        if (_quit_confirmation_needed) {
                            sys_info_update("Bye!");
                            _grid_run = false;
                        }
                        break;
                    }
                    case NO_KEY:
                    {
                        if (_quit_confirmation_needed) {
                            sys_info_update("");
                        }
                        break;
                    }
                    case KEY_ESC:
                    {
                        if (_quit_confirmation_needed) {
                            sys_info_update("");
                            _quit_confirmation_needed = false;
                        } else {
                            sys_info_update("Do you really want to quit? "
                                "([y]/n)");
                            _quit_confirmation_needed = true;
                        }
                        break;
                    }
                    default:
                    {
                        // no action
                    }
                }
            }
        }};
    }

    void sys_info_update(std::string&& si_)
    {
        std::unique_lock<std::mutex> ul{_display_mutex};
        _sys_info = si_;
        _print_info();
    }

    void print_info()
    {
        std::unique_lock<std::mutex> ul{_display_mutex};
        _print_info();
    }

    /// The '_' prefix means this method is NOT
    /// thread-safe.
    void _draw_object(ObjPack& op_)
    {
        ::mvwaddch(_grid
            ,c2cc(op_._pos.second)
            ,c2cc(op_._pos.first)
            , _objects[op_._key]);
        ::wrefresh(_grid);
    }

    /// The '_' prefix means this method is NOT
    /// thread-safe.
    void _update_info(InfoPack& ip_)
    {
        _info_data[ip_._key] = std::move(ip_._info);
        _print_info();
    }

    /// The '_' prefix means this method is NOT
    /// thread-safe.
    void _print_info()
    {
        // clear current info
        ::werase(_info_bar);

        // print client info
        ::wmove(_info_bar, 0, 0);

        ::wprintw(_info_bar, "[%d,%d]%c"
            , COLS, LINES, _info_separator);

        for (const auto& ik : _info_order) {
            auto info_part = _info_labels[ik];
            info_part += _info_data[ik];
            info_part += _info_separator;
            ::wprintw(_info_bar, info_part.c_str());
        }

        // print delay info
        auto delay_info = std::string{"delay: "};
        delay_info += std::to_string(_delay);
        delay_info += "us";
        delay_info += _info_separator;
        ::wprintw(_info_bar, delay_info.c_str());

        // print pause
        if (_paused) {
            auto pause_info = std::string{"paused!"};
            pause_info += _info_separator;
            ::wprintw(_info_bar, pause_info.c_str());
        }

        // print system info
        if (!_sys_info.empty()) {
            ::wprintw(_info_bar, "%s", _sys_info.c_str());
        }

        // display the new info
        ::wrefresh(_info_bar);
    }

    // Draw elements from Grid::_drawer_queue
    void draw()
    {
        while (_grid_run) {
            if (!_paused || _step || (0 != _init_steps)) {
                bool was_draw = false;
                std::unique_lock<std::mutex> drul{_drawer_mutex
                    , std::defer_lock};
                std::unique_lock<std::mutex> diul{_display_mutex
                    , std::defer_lock};
                std::lock(drul, diul);
                if (!_drawer_queue.empty()) {
                    auto& s = _drawer_queue.front();
                    switch (s._kind) {
                        case Kind::Obj:
                            _draw_object(boost::get<ObjPack>(s._data));
                            break;
                        case Kind::Info:
                            _update_info(boost::get<InfoPack>(s._data));
                            break;
                        default:
                            throw std::runtime_error("Unknown Kind!!!");
                    }
                    _drawer_queue.pop();
                    was_draw = true;
                }
                drul.unlock();
                diul.unlock();
                bool expected_step = true;
                _step.compare_exchange_weak(expected_step, false);
                if ((0 != _init_steps) && was_draw) {
                    --_init_steps;
                }
            }
            if (!_paused) {
                std::this_thread::sleep_for(time_t{_delay});
            }
        }
    }

public:
    Grid(object_map_t&& objects_, info_setup_t&& info_
        , const char info_separator_ = '#'
        , const time_t delay_change_ = time_t{50000}
        , const size_t init_steps_ = 0)
        : _objects{std::move(objects_)}
        , _color_mode{false}
        , _quit_confirmation_needed{false}
        , _info_bar{0}
        , _grid{0}
        , _delay{1000000}
        , _delay_change{delay_change_.count()}
        , _init_steps{init_steps_}
        , _info_separator{info_separator_}
        , _grid_run{true}
        , _paused{true}
        , _step{false}
    {
        for (const auto& i : info_) {
            const auto& ik = std::get<0>(i);
            _info_order.emplace_back(ik);
            _info_labels.emplace(ik, std::get<1>(i));
            _info_data.emplace(ik, std::get<2>(i));
        }
        
        // init curses
        if (!::initscr()) {
            throw std::runtime_error{"Failed to initialize curses!"};
        }

        // determine whether we have colour-support
        // in the current terminal
        if (::has_colors() == TRUE) {
            _color_mode = true;
            ::start_color();
            //::init_pair(1, COLOR_BLACK, TRUE_WHITE);
        }
        // disable line buffering
        ::raw();
        // do not print what the user types
        ::noecho();
        // enable arrow keys (among others...)
        ::keypad(stdscr, TRUE);
        // try to make blinking cursor invisible
        ::curs_set(0);

        // create windows
        _info_bar = new_window(1, COLS, 0, 0);
        _grid = new_window((LINES - 1), COLS, 1, 0);
        //if (_color_mode) {
        //    ::wbkgd(_info_bar, COLOR_PAIR(1));
        //    ::wbkgd(_grid, COLOR_PAIR(1));
        //}

        // init the info bar
        this->_print_info();

        // start the keyboard handler thread
        this->handle_keyboard();

        _drawer_thread = std::thread{[this] {
            this->draw();
        }};
    }
    ~Grid()
    {
        _drawer_thread.join();
        _keyboard_thread.join();
        ::delwin(_info_bar);
        ::delwin(_grid);
        ::endwin();
    }

    void draw(const ObjectKey key_, position_t&& p_)
    {
        auto s = Staff{Kind::Obj, ObjPack{key_, std::move(p_)}};
        std::unique_lock<std::mutex> ul{_drawer_mutex};
        _drawer_queue.push(s);
    }

    void update(const InfoKey key_, std::string&& info_)
    {
        auto s = Staff{Kind::Info, InfoPack{key_, std::move(info_)}};
        std::unique_lock<std::mutex> ul{_drawer_mutex};
        _drawer_queue.push(s);
    }
};

} // namespace Display
