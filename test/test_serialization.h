/*
    Copyright (c) 2016, Taiga Nomi
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
#include "picotest/picotest.h"
#include "testhelper.h"
#include "tiny_dnn/tiny_dnn.h"

namespace tiny_dnn {

TEST(serialization, sequential) {
    network<sequential> net1, net2;

    net1 << fully_connected_layer<tan_h>(10, 100)
         << dropout_layer(100, 0.3, net_phase::test)
         << fully_connected_layer<softmax>(100, 9)
         << convolutional_layer<tan_h>(3, 3, 3, 1, 1);

    auto json = net1.to_json();

    //std::cout << json;

    net2.from_json(json);

    EXPECT_EQ(net1.in_data_size(), net2.in_data_size());
    EXPECT_EQ(net1.layer_size(), net2.layer_size());

    EXPECT_EQ(net1[0]->in_shape(), net2[0]->in_shape());
    EXPECT_EQ(net1[1]->in_shape(), net2[1]->in_shape());
    EXPECT_EQ(net1[2]->in_shape(), net2[2]->in_shape());
    EXPECT_EQ(net1[3]->in_shape(), net2[3]->in_shape());

    EXPECT_EQ(net1[0]->layer_type(), net2[0]->layer_type());
    EXPECT_EQ(net1[1]->layer_type(), net2[1]->layer_type());
    EXPECT_EQ(net1[2]->layer_type(), net2[2]->layer_type());
    EXPECT_EQ(net1[3]->layer_type(), net2[3]->layer_type());

    EXPECT_FLOAT_EQ(net1.at<dropout_layer>(1).dropout_rate(), net2.at<dropout_layer>(1).dropout_rate());
}

TEST(serialization, graph) {
    network<graph> net1, net2;

    fully_connected_layer<tan_h> f1(3, 2);
    slice_layer s1(f1, slice_type::slice_samples, 2);
    fully_connected_layer<softmax> f2(2, 2);
    fully_connected_layer<elu> f3(2, 2);
    concat_layer c4(2, 2);

    f1 << s1;
    s1 << (f2, f3) << c4;

    construct_graph(net1, {&f1}, {&c4});

    auto json = net1.to_json();

    //std::cout << json;

    net2.from_json(json);

}

} // namespace tiny-dnn
