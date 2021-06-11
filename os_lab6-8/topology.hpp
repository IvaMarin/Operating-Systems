#include <list>
#include <stdexcept>

class topology {
private:
    std::list<std::list<int>> container;

public:
    void insert(int id, int parent_id) {
        if (parent_id == -1) { // if we were asked to create root node
            std::list<int> new_list;
            new_list.push_back(id);
            container.push_back(new_list);
        }
        else {
            int list_id = find(parent_id); // trying to find parent element first
            if (list_id == -1) { // if there is no such element with this id
                throw std::runtime_error("Wrong parent id");
            }
            auto it1 = container.begin();
            std::advance(it1, list_id); // set iterator to the position of parent list
            for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) { // for loop through the list 
                if (*it2 == parent_id) { // until we find the parent
                    it1->insert(++it2, id); // make id a chid of parent_id
                    return;
                }
            }
        }
    }

    int find(int id) {
        int cur_list_id = 0;
        for (auto it1 = container.begin(); it1 != container.end(); ++it1) { // for loop through all lists of root
            for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) { // for loop through all elments of current list
                if (*it2 == id) {
                    return cur_list_id;
                }
            }
            ++cur_list_id;
        }
        return -1;
    }

    void erase(int id) {
        int list_id = find(id);
        if (list_id == -1) {
            throw std::runtime_error("Wrong id");
        }
        auto it1 = container.begin();
        std::advance(it1, list_id);
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
            if (*it2 == id) {
                it1->erase(it2, it1->end());
                if (it1->empty()) { // if we erased the head of list 
                    container.erase(it1); // remove this list 
                }
                return;
            }
        }
    }

    int get_first_id(int list_id) { // additional method to find pointer to specified list 
        auto it1 = container.begin();
        std::advance(it1, list_id); // set iterator to the list we're looking for
        if (it1->begin() == it1->end()) { // if we reached the end it means it doesn't exist
            return -1;
        }
        return *(it1->begin());
    }
};