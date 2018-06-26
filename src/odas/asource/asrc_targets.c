
    /**
    * \file     asrc_targets.c
    * \author   François Grondin <francois.grondin2@usherbrooke.ca>
    * \version  2.0
    * \date     2018-06-19
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
    
    #include <asource/asrc_targets.h>

    asrc_targets_obj * asrc_targets_construct(const src_targets_cfg * src_targets_config, const msg_targets_cfg * msg_targets_config) {

        asrc_targets_obj * obj;

        obj = (asrc_targets_obj *) malloc(sizeof(asrc_targets_obj));

        obj->src_targets = src_targets_construct(src_targets_config, msg_targets_config);
        obj->in = (amsg_hops_obj *) NULL;
        obj->out = (amsg_targets_obj *) NULL;

        obj->thread = thread_construct(&asrc_targets_thread, (void *) obj);
        obj->thread_receive = thread_construct(&asrc_targets_thread_receive, (void *) obj);
        obj->thread_process = thread_construct(&asrc_targets_thread_process, (void *) obj);

        return obj;

    }

    void asrc_targets_destroy(asrc_targets_obj * obj) {

        src_targets_destroy(obj->src_targets);
        thread_destroy(obj->thread);
        thread_destroy(obj->thread_receive);
        thread_destroy(obj->thread_process);

        free((void *) obj);

    }

    void asrc_targets_connect(asrc_targets_obj * obj, amsg_hops_obj * in, amsg_targets_obj * out) {

        obj->in = in;
        obj->out = out;

    }

    void asrc_targets_disconnect(asrc_targets_obj * obj) {

        obj->in = (amsg_hops_obj *) NULL;
        obj->out = (amsg_targets_obj *) NULL;

    }

    void * asrc_targets_thread_receive(void * ptr) {

        asrc_targets_obj * obj;
        int rtnValue;

        obj = (asrc_targets_obj *) ptr;

        // Open the source
        src_targets_open(obj->src_targets);

        while(1) {

            rtnValue = src_targets_receive(obj->src_targets);

            if (rtnValue == -1) {
                break;
            }

        }

        // Close the source
        src_targets_close(obj->src_targets);                

    }

    void * asrc_targets_thread_process(void * ptr) {

        asrc_targets_obj * obj;
        msg_hops_obj * msg_hops_in;
        msg_targets_obj * msg_targets_out;
        int rtnValue;

        obj = (asrc_targets_obj *) ptr;

        if (obj->out == NULL) {
            printf("asrc_targets: nothing connected to output\n");
            exit(EXIT_FAILURE);
        }

        while(1) {

            // Pop a message, process, and push back
            msg_hops_in = amsg_hops_filled_pop(obj->in);
            msg_targets_out = amsg_targets_empty_pop(obj->out);
            src_targets_connect(obj->src_targets, msg_hops_in, msg_targets_out);
            rtnValue = src_targets_process(obj->src_targets);
            src_targets_disconnect(obj->src_targets);
            amsg_hops_empty_push(obj->in, msg_hops_in);
            amsg_targets_filled_push(obj->out, msg_targets_out);

            // If this is the last frame, rtnValue = -1
            if (rtnValue == -1) {
                break;
            }

        }

        // Send the "0" message, that kills all connected threads
        // that will consume this message
        msg_targets_out = amsg_targets_empty_pop(obj->out);
        msg_targets_zero(msg_targets_out);
        amsg_targets_filled_push(obj->out, msg_targets_out);

    }

    void * asrc_targets_thread(void * ptr) {

        asrc_targets_obj * obj;

        obj = (asrc_targets_obj *) ptr;

        if (obj->src_targets->port != 0) {
            thread_start(obj->thread_receive);
        }
        thread_start(obj->thread_process);

        if (obj->src_targets->port != 0) {
            thread_join(obj->thread_receive);
        }
        thread_join(obj->thread_process);

    }