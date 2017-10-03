/* Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserve.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#include "paddle/operators/adagrad_op.h"

namespace paddle {
namespace operators {

class AdagradOp : public framework::OperatorWithKernel {
 public:
  using framework::OperatorWithKernel::OperatorWithKernel;

 protected:
  void InferShape(framework::InferShapeContextBase *ctx) const override {
    PADDLE_ENFORCE(ctx->HasInput("param"),
                   "Input(param) of AdagradOp should not be null.");
    PADDLE_ENFORCE(ctx->HasInput("grad"),
                   "Input(grad) of AdagradOp should not be null.");
    PADDLE_ENFORCE(ctx->HasInput("moment"),
                   "Input(moment) of AdagradOp should not be null.");
    PADDLE_ENFORCE(ctx->HasInput("learning_rate"),
                   "Input(learning_rate) of AdagradOp should not be null.");

    PADDLE_ENFORCE(ctx->HasOutput("param_out"),
                   "Output(param_out) of AdagradOp should not be null.");
    PADDLE_ENFORCE(ctx->HasOutput("moment_out"),
                   "Output(moment_out) of AdagradOp should not be null.");

    auto lr_dims = ctx->GetInputDim("learning_rate");
    PADDLE_ENFORCE_EQ(framework::product(lr_dims), 1,
                      "learning_rate should have one element");
    auto param_dim = ctx->GetInputDim("param");
    PADDLE_ENFORCE_EQ(
        param_dim, ctx->GetInputDim("grad"),
        "Param and grad input of AdagradOp should have the same dimension.");
    PADDLE_ENFORCE_EQ(
        param_dim, ctx->GetInputDim("moment"),
        "Param and moment input of AdagradOp should have the same dimension.");

    ctx->SetOutputDim("param_out", param_dim);
    ctx->SetOutputDim("moment_out", param_dim);
  }
};

class AdagradOpMaker : public framework::OpProtoAndCheckerMaker {
 public:
  AdagradOpMaker(framework::OpProto *proto,
                 framework::OpAttrChecker *op_checker)
      : OpProtoAndCheckerMaker(proto, op_checker) {
    AddInput("param", "Input parameter");
    AddInput("grad", "Input gradient");
    AddInput("moment", "Second moment");
    AddInput("learning_rate", "learning rate of adagrad");

    AddOutput("param_out", "Output parameter");
    AddOutput("moment_out", "Output second moment");

    AddAttr<float>("epsilon", "Constant for numerical stability");
    AddComment(R"DOC(

Adaptive Gradient Algorithm (Adagrad).

moment_out = moment + grad * grad
param_out = param - learning_rate * grad / (sqrt(moment_out) + epsilon)

The original paper(http://www.jmlr.org/papers/volume12/duchi11a/duchi11a.pdf)
does not have the epsilon attribute. It is added here for numerical stability 
by avoiding division by zero.

)DOC");
  }
};
}  // namespace operators
}  // namespace paddle

namespace ops = paddle::operators;
REGISTER_OP_WITHOUT_GRADIENT(adagrad, ops::AdagradOp, ops::AdagradOpMaker);
REGISTER_OP_CPU_KERNEL(adagrad,
                       ops::AdagradOpKernel<paddle::platform::CPUPlace, float>);
