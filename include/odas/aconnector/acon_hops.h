#ifndef __ODAS_ACONNECTOR_HOPS
#define __ODAS_ACONNECTOR_HOPS

    /**
    * \file     acon_hops.h
    * \author   François Grondin <francois.grondin2@usherbrooke.ca>
    * \version  2.0
    * \date     2018-03-18
    * \copyright
    *
    * This program is free software: you can redistribute it and/or modify
    * it under the terms of the GNU General Public License as published by
    * the Free Software Foundation, either version 3 of the License, or
    * (at your option) any later version.
    *
    * This program is distributed in the hope that it will be useful,
    * but WITHOUT ANY WARRANTY; without even the implied warranty of
    * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    * GNU General Public License for more details.
    * 
    * You should have received a copy of the GNU General Public License
    * along with this program.  If not, see <http://www.gnu.org/licenses/>.
    *
    */

    #include "../connector/con_hops.h"
    #include "../amessage/amsg_hops.h"
    #include "../message/msg_hops.h"
    #include "../general/thread.h"

    #include <stdlib.h>
    #include <stdio.h>

    typedef struct acon_hops_obj {

        amsg_hops_obj * in;
        amsg_hops_obj ** outs;
        con_hops_obj * con_hops;
        thread_obj * thread;

    } acon_hops_obj;

    acon_hops_obj * acon_hops_construct(const unsigned int nMessages, const unsigned int nOuts, const msg_hops_cfg * msg_hops_config);

    void acon_hops_destroy(acon_hops_obj * obj);

    void * acon_hops_thread(void * ptr);

#endif