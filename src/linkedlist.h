/*
MIT License
Copyright (c) 2019 Taehun Kang(agongee123@gmail.com), Jaewoo Pyo(jwpyo98@gmail.com) and Bogyeong Park(parkbo0201@gmail.com)
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
This class supports to use linkedlist as data type
*/
#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include<iostream>

namespace linkedlist
{

typedef struct NODE
{
    std::string content = "";
    struct NODE *prev_ptr = NULL, *next_ptr = NULL;
} node;

class List
{
private:
    node* head;
    int length;

public:
    List();
    ~List();

    void push_front(std::string content);
    void push_back(std::string content);
    std::string pop_front();
    std::string pop_back();

    int find(std::string content);
    node* at(int index);
    std::string get(int index)
    {
        return this->at(index)->content;
    }
    void set(int index, std::string content)
    {
        this->at(index)->content = content;
    };
    bool del(int index);

    bool is_empty()
    {
        return head==NULL;
    }
    int get_length()
    {
        return length;
    }
    std::string print();
};
}

#include "linkedlist.cpp"
#endif // LINKEDLIST_H
