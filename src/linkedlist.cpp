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

#ifndef LINKEDLIST_CPP
#define LINKEDLIST_CPP

#include "linkedlist.h"

//default constructor of the list
inline linkedlist::List::List()
{
    head = NULL;
    length = 0;
}

//deconstructor of linkedlist
inline linkedlist::List::~List()
{
    while(!is_empty())
        pop_back();
}

//push element to head of the list
inline void linkedlist::List::push_front(std::string content)
{
    node *element = new node();
    element->content = content;

    if(is_empty())
    {
        element->next_ptr = element;
        element->prev_ptr = element;

        head = element;
    }
    else
    {
        element->next_ptr = head;
        element->prev_ptr = head->prev_ptr;
        head->prev_ptr->next_ptr = element;
        head->prev_ptr = element;

        head = element;
    }

    length += 1;
}

//push element to tail of the list
inline void linkedlist::List::push_back(std::string content)
{
    node *element = new node();
    element->content = content;

    if(is_empty())
    {
        element->next_ptr = element;
        element->prev_ptr = element;

        head = element;
    }
    else
    {
        element->next_ptr = head;
        element->prev_ptr = head->prev_ptr;
        head->prev_ptr->next_ptr = element;
        head->prev_ptr = element;
    }

    length += 1;
}

//pop first element, and return content of the element
inline std::string linkedlist::List::pop_front()
{
    if(is_empty())
    {
        return "";
    }
    else if(head->next_ptr == head)
    {
        std::string res = head->content;

        head = NULL;
        delete head;
        length -= 1;
        return res;
    }
    else
    {
        std::string res = head->content;

        head->next_ptr->prev_ptr = head->prev_ptr;
        head->prev_ptr->next_ptr = head->next_ptr;

        node *bef_head = head;
        head = head->next_ptr;
        delete bef_head;
        length -= 1;
        return res;
    }
}

//pop last element, and return content of the element
inline std::string linkedlist::List::pop_back()
{
    if(is_empty())
    {
        return "";
    }
    else if(head->next_ptr == head)
    {
        std::string res = head->content;

        head = NULL;
        delete head;
        length -= 1;
        return res;
    }
    else
    {
        node *tail = head->prev_ptr;
        std::string res = tail->content;

        tail->next_ptr->prev_ptr = tail->prev_ptr;
        tail->prev_ptr->next_ptr = tail->next_ptr;

        delete tail;
        length -= 1;
        return res;
    }
}

//return index of the content; if not exist, return -1
inline int linkedlist::List::find(std::string content)
{
    if(is_empty())
        return -1;

    node *res = head;
    int index = 0;

    while(true)
    {
        if(res->content == content)
            return index;

        res = res->next_ptr;
        index++;
        if(res == head)
            break;
    }

    return -1;
}

//return ptr of the index-th element of the list
inline linkedlist::node* linkedlist::List::at(int index)
{
    if(is_empty())
        return NULL;

    node *res = head;

    if(index >= 0)
        for(int i = 0; i < index % length; i++)
            res = res->next_ptr;
    else
        for(int i = 0; i < (-index) % length; i++)
            res = res->prev_ptr;

    return res;
}

//delete the index-th element of the list
inline bool linkedlist::List::del(int index)
{
    node *trg = at(index);
    if(trg == NULL)
        return false;

    if(trg->next_ptr == trg)
    {
        head = NULL;
        delete trg;
        length -= 1;
    }
    else
    {
        if(trg == head)
            head = head->next_ptr;
        trg->next_ptr->prev_ptr = trg->prev_ptr;
        trg->prev_ptr->next_ptr = trg->next_ptr;
        delete trg;
        length -= 1;
    }

    return true;
}

//declare print format of the list
inline std::string linkedlist::List::print()
{
    if(is_empty())
        return "";

    std::string str = "";
    str += head->content;

    for(linkedlist::node *it = head->next_ptr; it != head; it = it->next_ptr)
        str = str + "," + it->content;

    return str;
}
#endif
