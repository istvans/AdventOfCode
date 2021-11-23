/** --- Day 9: All in a Single Night ---

Every year, Santa manages to deliver all of his presents in a single night.

This year, however, he has some new locations to visit; his elves have provided him the distances
between every pair of locations. He can start and end at any two (different) locations he wants,
but he must visit each location exactly once. What is the shortest distance he can travel to
achieve this?

For example, given the following distances:

London to Dublin = 464
London to Belfast = 518
Dublin to Belfast = 141
The possible routes are therefore:

Dublin -> London -> Belfast = 982
London -> Dublin -> Belfast = 605
London -> Belfast -> Dublin = 659
Dublin -> Belfast -> London = 659
Belfast -> Dublin -> London = 605
Belfast -> London -> Dublin = 982
The shortest of these is London -> Dublin -> Belfast = 605, and so the answer is 605 in this
example.

What is the distance of the shortest route?
https://adventofcode.com/2015/day/9
*/
#include "macros.h"

CLANG_DIAG_IGNORE_BOOST_WARNINGS()
#include <boost/graph/adjacency_list.hpp>
#include <boost/program_options.hpp>
CLANG_DIAG_POP()

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <numeric>
#include <regex>
#include <string>
#include <tuple>
#include <utility>


using vertex_property_t = boost::property<boost::vertex_name_t, std::string>;
using raw_distance_t = unsigned long;
using graph_distance_t = boost::property<boost::edge_weight_t, raw_distance_t>;
using graph_t = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
    vertex_property_t, graph_distance_t>;
using vertices_size_t = graph_t::vertices_size_type;
using vertex_t = graph_t::vertex_descriptor;
using optional_vertex_t = std::optional<vertex_t>;
using vertices_t = std::vector<vertex_t>;


class path_t
{
    using optional_graph_t = std::optional<graph_t>;

    optional_graph_t optional_graph = std::nullopt;
    raw_distance_t distance = 0;
    vertices_size_t max_num_vertices;
    vertices_t vertices;
    bool evaluated = false;

public:
    path_t(vertices_size_t num_vertices, raw_distance_t distance_ = 0)
        : distance{distance_}, max_num_vertices{num_vertices}, vertices(), evaluated{false}
    {
        vertices.reserve(num_vertices);
    }

    path_t(const path_t&) = default;
    path_t& operator=(const path_t&) = default;
    path_t(path_t&&) = default;
    path_t& operator=(path_t&&) = default;

    bool is_hamiltonian() const
    {
        return this->max_num_vertices == vertices.size();
    }

    bool contains(vertex_t vertex) const
    {
        const auto& v = this->vertices;
        const auto cit = std::find(v.cbegin(), v.cend(), vertex);
        return (cit != v.cend());
    }

    void append(vertex_t vertex, raw_distance_t distance_from_path)
    {
        auto& v = this->vertices;
        v.push_back(vertex);
        if (v.size() > 1) {
            this->distance += distance_from_path;
        } else {
            this->distance = 0;
        }
    }

    raw_distance_t get_distance() const
    {
        return this->distance;
    }

    vertices_size_t num_vertices() const
    {
        return this->vertices.size();
    }

    bool get_max_num_vertices() const
    {
        return this->max_num_vertices;
    }

    bool was_evaluated() const
    {
        return this->evaluated;
    }

    optional_vertex_t last_vertex() const
    {
        const auto& v = this->vertices;
        if (v.empty())
        {
            return std::nullopt;
        } else {
            return v.back();
        }
    }

    void set_evaluated()
    {
        this->evaluated = true;
    }

    void add_vertex_names_from(const graph_t& graph)
    {
        this->optional_graph = graph;
    }

    void remove_vertex_names()
    {
        this->optional_graph.reset();
    }

    template <class Out>
    friend Out& operator<<(Out&, const path_t&);
};

template <class Out>
Out& operator<<(Out& out, const path_t& path)
{
    auto& optional_graph = path.optional_graph;

    auto buffer = std::string("#");
    buffer += std::to_string(path.num_vertices());

    buffer += " distance: ";
    buffer += std::to_string(path.get_distance());

    if (path.optional_graph.has_value()) {
        const auto& vertex_names = boost::get(boost::vertex_name, optional_graph.value());
        for (auto vertex: path.vertices) {
            const auto& name = vertex_names[vertex];
            buffer += ' ';
            buffer += name;
        }
    } else {
        for (auto vertex: path.vertices) {
            buffer += ' ';
            buffer += std::to_string(vertex);
        }
    }

    return out << buffer;
}

using paths_t = std::list<path_t>;


template <class File>
static auto create_graph_from(File& file, const std::string& filename)
{
    std::string line;
    const auto src_dst_distance_regex = std::regex(R"##(^(\S+)\s+to\s+(\S+)\s+=\s+(\d+))##");
    std::smatch match;

    using edge_t = std::pair<vertices_size_t, vertices_size_t>;
    using edges_t = std::vector<edge_t>;

    vertices_size_t num_vertices = 0;
    auto edges = edges_t();
    auto distances = std::vector<graph_distance_t>();
    auto input_vertices = std::unordered_map<std::string, vertices_size_t>();
    while (std::getline(file, line)) {
        if (std::regex_search(line, match, src_dst_distance_regex)) {
            const auto src_vertex_name = match[1].str();
            auto result = input_vertices.try_emplace(src_vertex_name, num_vertices);
            auto iterator = result.first;
            auto inserted = result.second;
            if (inserted) {
                ++num_vertices;
            }
            const auto src_vertex_index = iterator->second;

            const auto dst_vertex_name = match[2].str();
            result = input_vertices.try_emplace(dst_vertex_name, num_vertices);
            iterator = result.first;
            inserted = result.second;
            if (inserted) {
                ++num_vertices;
            }
            const auto dst_vertex_index = iterator->second;

            edges.emplace_back(src_vertex_index, dst_vertex_index);

            const unsigned long distance = std::stoul(match[3].str());
            distances.push_back(distance);
        } else {
            throw std::invalid_argument(
                std::string("Invalid input file:'") + filename + "' with line:'"+ line + '\''
            );
        }
    }

    auto graph = graph_t(edges.cbegin(), edges.cend(), distances.cbegin(), num_vertices);
    auto vertex_names = boost::get(boost::vertex_name, graph);
    for (const auto& [name, index]: input_vertices) {
        boost::put(vertex_names, index, name);
    }

    return graph;
}


/// Return the new paths which are built with the directly reachable, not-yet visited vertices.
static auto find_new_paths(const path_t& active_path, const graph_t& graph)
{
    assert(active_path.num_vertices() > 0);

    auto new_paths = paths_t();

    const auto last_vertex = active_path.last_vertex().value();
    auto [out_edge_it, out_edges_end] = boost::out_edges(last_vertex, graph);
    for (; out_edge_it != out_edges_end; ++out_edge_it) {
        const auto edge = *out_edge_it;
        const auto tgt_vertex = boost::target(edge, graph);

        if (active_path.contains(tgt_vertex)) {
            continue;
        }

        const auto tgt_vertex_distance = boost::get(boost::edge_weight, graph, edge);
        auto new_path = active_path;
        new_path.append(tgt_vertex, tgt_vertex_distance);

        new_paths.push_back(new_path);
    }

    return new_paths;
}


static auto find_the_shortest_and_longest_hamiltioninan_paths_in(const graph_t& graph)
{
    const auto num_vertices = boost::num_vertices(graph);

    auto paths = paths_t(num_vertices, path_t(num_vertices));
    const auto vertices = boost::vertices(graph);
    auto vertex_it = vertices.first;
    for (auto& path: paths) {
        path.append(*vertex_it, 0);
        ++vertex_it;
    }

    auto shortest_hamiltonian_path = path_t(
        num_vertices,
        std::numeric_limits<raw_distance_t>::max()
    );
    auto longest_hamiltonian_path = path_t(num_vertices, 0);

    while (!paths.empty()) {
        auto& active_path = paths.front();
        if (active_path.num_vertices() == num_vertices) {
            active_path.add_vertex_names_from(graph);
            std::cout << active_path << '\n';
        }

        if (active_path.is_hamiltonian()) {
            if (active_path.get_distance() < shortest_hamiltonian_path.get_distance()) {
                shortest_hamiltonian_path = active_path;
            }
            if (active_path.get_distance() > longest_hamiltonian_path.get_distance()) {
                longest_hamiltonian_path = active_path;
            }
            active_path.set_evaluated();
        } else {
            auto new_paths = find_new_paths(active_path, graph);

            if (new_paths.empty()) {
                active_path.set_evaluated();
            } else {
                active_path = std::move(new_paths.front());
                new_paths.pop_front();
                paths.splice(paths.end(), std::move(new_paths));
            }
        }

        if (active_path.was_evaluated()) {
            paths.pop_front();
        }
    }

    shortest_hamiltonian_path.add_vertex_names_from(graph);
    longest_hamiltonian_path.add_vertex_names_from(graph);

    return std::make_tuple(shortest_hamiltonian_path, longest_hamiltonian_path);
}

int main(int argc_, char** argv_)
{
    namespace po = boost::program_options;
    std::string input_filename;

    po::options_description description("Options");
    description.add_options()
        ("input,i", po::value<std::string>(&input_filename), "input file")
        ("help,h", "show this help message")
        ;

    po::variables_map vm;
    int exit_code = 0;
    try {
        po::store(po::parse_command_line(argc_, argv_, description), vm);
    } catch (const std::exception& ex) {
        std::cout << description << " - " << ex.what() << std::endl;
        exit_code = 1;
    }

    if (exit_code == 0) {
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << description << std::endl;
        } else if (!vm.count("input")) {
            std::cout << description << std::endl;
            exit_code = 2;
        }
    }

    if (exit_code == 0) {
        if (auto file = std::fstream{input_filename, std::ios_base::in}) {
            auto graph = create_graph_from(file, input_filename);

            const auto [shortest_hamiltonian_path, longest_hamiltonian_path] =
                find_the_shortest_and_longest_hamiltioninan_paths_in(graph);

            std::cout << "=================================================================================\n";
            std::cout << "What is the distance of the shortest route?\n" << shortest_hamiltonian_path << '\n';
            std::cout << "\n";
            std::cout << "What is the distance of the longest route?\n" << longest_hamiltonian_path << '\n';
        } else {
            std::cerr << "cannot open input file \'" << input_filename << '\'' << std::endl;
            exit_code = 3;
        }
    }

    return exit_code;
}
