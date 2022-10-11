#ifndef SRC_AUTO_DIFF_AUTO_DIFF_H_
#define SRC_AUTO_DIFF_AUTO_DIFF_H_

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <vector>

#define UNUSED(x) (void)(x)

namespace ad {
class OpBase;
class VariableImpl;
class Variable {
 public:
  Variable() = default;

  explicit Variable(float value);

  Variable operator+(const Variable& rhs) const;
  Variable operator-(const Variable& rhs) const;
  Variable operator*(const Variable& rhs) const;
  Variable operator/(const Variable& rhs) const;
  Variable operator-() const;
  Variable Sin() const;
  Variable Cos() const;
  Variable Log() const;
  Variable Exp() const;

  explicit operator bool() { return variable_ != nullptr; }

  float Value();

  std::string Name() const;

  Variable& Inputs(std::size_t i);

  std::size_t NumInputs() const;

  const Variable& Inputs(std::size_t i) const;

  std::shared_ptr<OpBase> Op();

  const std::shared_ptr<OpBase> Op() const;

  void Backpropagation();

  Variable GetAdjoint();
  const Variable GetAdjoint() const;

  bool operator==(const Variable& rhs) { return rhs.variable_ == variable_; }

  static void PrintAllVariablesInPool();
  static void ZeroGradient();

 private:
  Variable(VariableImpl* var);

 private:
  VariableImpl* variable_ = nullptr;
};

class VariableImpl {
  friend Variable;

 public:
  VariableImpl(const VariableImpl&) = delete;
  VariableImpl& operator=(const VariableImpl&) = delete;

  static VariableImpl* NewVariable(float value) {
    std::string name = "v" + std::to_string(variable_pool_.size());
    variable_pool_[name] =
        std::unique_ptr<VariableImpl>(new VariableImpl{name, value});
    return variable_pool_[name].get();
  }
  static std::map<std::string, std::unique_ptr<VariableImpl>> variable_pool_;

 private:
  explicit VariableImpl(const std::string& name, float value = .0F)
      : name_(name), cached_value_(value) {}
  std::vector<Variable> inputs_;
  Variable adjoint_;
  std::vector<Variable> adjoint_vec_;
  std::shared_ptr<OpBase> op_;
  std::string name_;
  float cached_value_;
};

std::map<std::string, std::unique_ptr<VariableImpl>>
    VariableImpl::variable_pool_;

class OpBase {
 public:
  OpBase(const char* name) : op_name_(name) {}
  virtual float Compute(std::vector<Variable>& inputs) const = 0;
  virtual std::vector<Variable> Gradient(const std::vector<Variable> inputs,
                                         Variable out_adjoint) const = 0;
  std::string GetName() const { return op_name_; };
  virtual ~OpBase() = default;

 private:
  std::string op_name_;
};

class PlusOp : public OpBase {
  using OpBase::OpBase;

 public:
  float Compute(std::vector<Variable>& inputs) const final;
  std::vector<Variable> Gradient(const std::vector<Variable> inputs,
                                 Variable out_adjoint) const final;
};

class MinusOp : public OpBase {
  using OpBase::OpBase;

 public:
  float Compute(std::vector<Variable>& inputs) const final;
  std::vector<Variable> Gradient(const std::vector<Variable> inputs,
                                 Variable out_adjoint) const final;
};

class MultipleOp : public OpBase {
  using OpBase::OpBase;

 public:
  float Compute(std::vector<Variable>& inputs) const final;
  std::vector<Variable> Gradient(const std::vector<Variable> inputs,
                                 Variable out_adjoint) const final;
};

class DivideOp : public OpBase {
  using OpBase::OpBase;

 public:
  float Compute(std::vector<Variable>& inputs) const final;
  std::vector<Variable> Gradient(const std::vector<Variable> inputs,
                                 Variable out_adjoint) const final;
};

class SinOp : public OpBase {
  using OpBase::OpBase;

 public:
  float Compute(std::vector<Variable>& inputs) const final;
  std::vector<Variable> Gradient(const std::vector<Variable> inputs,
                                 Variable out_adjoint) const final;
};

class CosOp : public OpBase {
  using OpBase::OpBase;

 public:
  float Compute(std::vector<Variable>& inputs) const final;
  std::vector<Variable> Gradient(const std::vector<Variable> inputs,
                                 Variable out_adjoint) const final;
};

class LogOp : public OpBase {
  using OpBase::OpBase;

 public:
  float Compute(std::vector<Variable>& inputs) const final;
  std::vector<Variable> Gradient(const std::vector<Variable> inputs,
                                 Variable out_adjoint) const final;
};

class ExpOp : public OpBase {
  using OpBase::OpBase;

 public:
  float Compute(std::vector<Variable>& inputs) const final;
  std::vector<Variable> Gradient(const std::vector<Variable> inputs,
                                 Variable out_adjoint) const final;
};

class Negitive : public OpBase {
  using OpBase::OpBase;

 public:
  float Compute(std::vector<Variable>& inputs) const final;
  std::vector<Variable> Gradient(const std::vector<Variable> inputs,
                                 Variable out_adjoint) const final;
};

Variable::Variable(VariableImpl* var) : variable_(var) {}

Variable::Variable(float value) {
  variable_ = VariableImpl::NewVariable(value);
}

void Variable::PrintAllVariablesInPool() {
  for (const auto& var : VariableImpl::variable_pool_) {
    std::cout << var.second->name_;
    if (var.second->inputs_.size() == 0) {
      std::cout << std::endl;
    } else if (var.second->inputs_.size() == 1) {
      std::cout << ": " << var.second->op_->GetName()
                << var.second->inputs_[0].Name() << std::endl;
    } else if (var.second->inputs_.size() == 2) {
      std::cout << ": " << var.second->inputs_[0].Name()
                << var.second->op_->GetName() << var.second->inputs_[1].Name()
                << std::endl;
    }
  }
}

void Variable::ZeroGradient() {
  for (auto& var : VariableImpl::variable_pool_) {
    var.second->adjoint_vec_.clear();
  }
}

void Variable::Backpropagation() {
  std::queue<Variable> q;
  q.push(*this);
  std::vector<Variable> sorted_vec;
  while (!q.empty()) {
    auto ref = q.front();
    for (auto& input : ref.variable_->inputs_) {
      q.push(input);
    }
    auto iter = std::find(sorted_vec.begin(), sorted_vec.end(), ref);
    if (iter != sorted_vec.end()) {
      sorted_vec.erase(iter);
    }
    sorted_vec.push_back(ref);
    q.pop();
  }
  // for (auto& ref : sorted_vec) {
  //   std::cout << ref.Name() << ", ";
  // }
  std::cout << std::endl;
  auto all_refs = sorted_vec;
  for (std::size_t i = 0; i < all_refs.size(); ++i) {
    if (all_refs[i].variable_->adjoint_vec_.empty()) {
      all_refs[i].variable_->adjoint_vec_.emplace_back(1.0F);
    }
    all_refs[i].variable_->adjoint_ =
        all_refs[i].variable_->adjoint_vec_.front();
    for (std::size_t j = 1; j < all_refs[i].variable_->adjoint_vec_.size();
         ++j) {
      all_refs[i].variable_->adjoint_ = all_refs[i].variable_->adjoint_ +
                                        all_refs[i].variable_->adjoint_vec_[j];
    }
    if (all_refs[i].Op() == nullptr) {
      continue;
    }
    auto gradients = all_refs[i].Op()->Gradient(
        all_refs[i].variable_->inputs_, all_refs[i].variable_->adjoint_);
    for (std::size_t j = 0; j < gradients.size(); ++j) {
      all_refs[i].variable_->inputs_[j].variable_->adjoint_vec_.push_back(
          gradients[j]);
    }
  }
}

Variable Variable::operator+(const Variable& rhs) const {
  auto op = std::make_shared<PlusOp>("@plus@");
  auto var_ref = Variable(.0F);
  var_ref.variable_->op_ = op;
  var_ref.variable_->inputs_.emplace_back(*this);
  var_ref.variable_->inputs_.emplace_back(rhs);
  return var_ref;
}

Variable Variable::operator-(const Variable& rhs) const {
  auto op = std::make_shared<MinusOp>("@minus@");
  auto var_ref = Variable(.0F);
  var_ref.variable_->op_ = op;
  var_ref.variable_->inputs_.emplace_back(*this);
  var_ref.variable_->inputs_.emplace_back(rhs);
  return var_ref;
}

Variable Variable::operator*(const Variable& rhs) const {
  auto op = std::make_shared<MultipleOp>("@multiple@");
  auto var_ref = Variable(.0F);
  var_ref.variable_->op_ = op;
  var_ref.variable_->inputs_.emplace_back(*this);
  var_ref.variable_->inputs_.emplace_back(rhs);
  return var_ref;
}

Variable Variable::operator/(const Variable& rhs) const {
  auto op = std::make_shared<DivideOp>("@divide@");
  auto var_ref = Variable(.0F);
  var_ref.variable_->op_ = op;
  var_ref.variable_->inputs_.emplace_back(*this);
  var_ref.variable_->inputs_.emplace_back(rhs);
  return var_ref;
}

Variable Variable::operator-() const {
  auto op = std::make_shared<Negitive>("negitive@");
  auto var_ref = Variable(.0F);
  var_ref.variable_->op_ = op;
  var_ref.variable_->inputs_.emplace_back(*this);
  return var_ref;
}

Variable Variable::Sin() const {
  auto op = std::make_shared<SinOp>("sin@");
  auto var_ref = Variable(.0F);
  var_ref.variable_->op_ = op;
  var_ref.variable_->inputs_.emplace_back(*this);
  return var_ref;
}

Variable Variable::Cos() const {
  auto op = std::make_shared<CosOp>("cos@");
  auto var_ref = Variable(.0F);
  var_ref.variable_->op_ = op;
  var_ref.variable_->inputs_.emplace_back(*this);
  return var_ref;
}

Variable Variable::Log() const {
  auto op = std::make_shared<LogOp>("log@");
  auto var_ref = Variable(.0F);
  var_ref.variable_->op_ = op;
  var_ref.variable_->inputs_.emplace_back(*this);
  return var_ref;
}

Variable Variable::Exp() const {
  auto op = std::make_shared<ExpOp>("exp@");
  auto var_ref = Variable(.0F);
  var_ref.variable_->op_ = op;
  var_ref.variable_->inputs_.emplace_back(*this);
  return var_ref;
}

float Variable::Value() {
  if (variable_->op_) {
    variable_->cached_value_ = variable_->op_->Compute(variable_->inputs_);
  }
  return variable_->cached_value_;
}

std::string Variable::Name() const { return variable_->name_; }

Variable& Variable::Inputs(std::size_t i) { return variable_->inputs_[i]; }

std::size_t Variable::NumInputs() const { return variable_->inputs_.size(); }

const Variable& Variable::Inputs(std::size_t i) const {
  return variable_->inputs_[i];
}

std::shared_ptr<OpBase> Variable::Op() { return variable_->op_; }

const std::shared_ptr<OpBase> Variable::Op() const { return variable_->op_; }

Variable Variable::GetAdjoint() {
  if (variable_->adjoint_) {
    return variable_->adjoint_;
  }
  throw std::invalid_argument("Run backward before get adjoint");
}

const Variable Variable::GetAdjoint() const {
  if (variable_->adjoint_) {
    return variable_->adjoint_;
  }
  throw std::invalid_argument("Run backward before get adjoint");
}

float PlusOp::Compute(std::vector<Variable>& inputs) const {
  return inputs[0].Value() + inputs[1].Value();
}

std::vector<Variable> PlusOp::Gradient(const std::vector<Variable> inputs,
                                       Variable out_adjoint) const {
  UNUSED(inputs);
  return {out_adjoint, out_adjoint};
}

float MinusOp::Compute(std::vector<Variable>& inputs) const {
  return inputs[0].Value() - inputs[1].Value();
}

std::vector<Variable> MinusOp::Gradient(const std::vector<Variable> inputs,
                                        Variable out_adjoint) const {
  UNUSED(inputs);
  return {out_adjoint, -out_adjoint};
}

float MultipleOp::Compute(std::vector<Variable>& inputs) const {
  return inputs[0].Value() * inputs[1].Value();
}

std::vector<Variable> MultipleOp::Gradient(const std::vector<Variable> inputs,
                                           Variable out_adjoint) const {
  return {out_adjoint * inputs[1], out_adjoint * inputs[0]};
}

float DivideOp::Compute(std::vector<Variable>& inputs) const {
  return inputs[0].Value() / inputs[1].Value();
}

std::vector<Variable> DivideOp::Gradient(const std::vector<Variable> inputs,
                                         Variable out_adjoint) const {
  return {out_adjoint / inputs[1],
          -inputs[0] * out_adjoint / (inputs[1] * inputs[1])};
}

float SinOp::Compute(std::vector<Variable>& inputs) const {
  return std::sin(inputs[0].Value());
}

std::vector<Variable> SinOp::Gradient(const std::vector<Variable> inputs,
                                      Variable out_adjoint) const {
  return {out_adjoint * inputs[0].Cos()};
}

float CosOp::Compute(std::vector<Variable>& inputs) const {
  return std::cos(inputs[0].Value());
}

std::vector<Variable> CosOp::Gradient(const std::vector<Variable> inputs,
                                      Variable out_adjoint) const {
  return {out_adjoint * -inputs[0].Sin()};
}

float LogOp::Compute(std::vector<Variable>& inputs) const {
  return std::log(inputs[0].Value());
}

std::vector<Variable> LogOp::Gradient(const std::vector<Variable> inputs,
                                      Variable out_adjoint) const {
  return {out_adjoint / inputs[0]};
}

float ExpOp::Compute(std::vector<Variable>& inputs) const {
  return std::exp(inputs[0].Value());
}

std::vector<Variable> ExpOp::Gradient(const std::vector<Variable> inputs,
                                      Variable out_adjoint) const {
  return {out_adjoint * inputs[0].Exp()};
}

float Negitive::Compute(std::vector<Variable>& inputs) const {
  return -inputs[0].Value();
}

std::vector<Variable> Negitive::Gradient(const std::vector<Variable> inputs,
                                         Variable out_adjoint) const {
  UNUSED(inputs);
  return {-out_adjoint};
}

}  // namespace ad
#endif  // SRC_AUTO_DIFF_AUTO_DIFF_H_
