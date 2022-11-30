//
//  MachineLearning.h
//  Core Lib
//
//  Created by Rasmus Anthin on 2022-11-28.
//

#pragma once
#include "StlUtils.h"
#include <optional>
#include <memory>


namespace ml
{

  namespace ann
  {
  
    enum class PhiType
    {
      BinaryStep,
      Heaviside_BinaryStep,
      Linear,
      Sigmoid,
      Tanh,
      ReLU,
      Parametric_ReLU,
      Leaky_ReLU,
      Parametric_Leaky_ReLU,
      ELU,
      Swish,
      GELU,
      SELU,
    };
  
    float phi(float z, PhiType type, float a = 1.f, float k = 1.f, float l = 1.1f)
    {
      switch (type)
      {
        case PhiType::BinaryStep: return z < 0 ? 0 : 1;
        case PhiType::Heaviside_BinaryStep: return z <= 0 ? 0 : 1;
        case PhiType::Linear: return z;
        case PhiType::Sigmoid: return 1./(1 + std::exp(-z));
        case PhiType::Tanh: return std::tanh(z);
        case PhiType::ReLU: return std::max(0.f, z);
        case PhiType::Parametric_ReLU: return std::max(0.f, k*z + l);
        case PhiType::Leaky_ReLU: return std::max(0.1f*z, z);
        case PhiType::Parametric_Leaky_ReLU: return std::max(a*(k*z + l), k*z + l);
        case PhiType::ELU: return z < 0 ? a*(std::exp(z) - 1) : z;
        case PhiType::Swish: return z*phi(z, PhiType::Sigmoid);
        //case PhiType::GELU: return 0.5*z*(1 + std::tanh(M_2_SQRTPI*M_SQRT1_2*(z + 0.044715*math::cube(z))));
        case PhiType::GELU: return 0.5*z*(1 + std::erf(z/M_SQRT2));
        case PhiType::SELU: return l*phi(z, PhiType::ELU, a, k, l);
      }
    }
  
    float phi_diff(float z, PhiType type, float a = 1.f, float k = 1.f, float l = 1.1f)
    {
      switch (type)
      {
        case PhiType::BinaryStep: return 0.f; // Actually inf at 0- and 0 everywhere else.
        case PhiType::Heaviside_BinaryStep: return 0.f; // Actually inf at 0+ and 0 everywhere else.
        case PhiType::Linear: return 1.f;
        case PhiType::Sigmoid:
        {
          auto s = phi(z, type, a, k, l);
          return s * (1 - s);
        }
        case PhiType::Tanh:
        {
          auto th = phi(z, type, a, k, l);
          return 1 - math::sq(th);
        }
        case PhiType::ReLU: return z < 0 ? 0 : 1;
        case PhiType::Parametric_ReLU: return z < -l/k ? 0 : k;
        case PhiType::Leaky_ReLU: return z < 0 ? 0.1 : 1;
        case PhiType::Parametric_Leaky_ReLU: return z < -l/k ? a*k : k;
        case PhiType::ELU: return z < 0 ? phi(z, type, a, k, l) + a : 1;
        case PhiType::Swish:
        {
          auto sw = phi(z, type, a, k, l);
          auto sig = phi(z, PhiType::Sigmoid, a, k, l);
          return sw + sig * (1 - sw);
        }
        case PhiType::GELU:
        {
          //auto z3 = math::cube(z);
          //auto sech = [](float v) { return std::sqrt(1 - math::sq(std::tanh(v))); };
          //auto b = 0.797885f*z + 0.0356774f*z3;
          //return 0.5f + (0.398942f*z + 0.0535161f*z3)*math::sq(sech(b)) + 0.5f*std::tanh(b);
          
          // 1/2*(erf(z/sqrt(2)) + 1) + exp(-z^2/2)*z/(sqrt(2*pi))
          static const auto c_1_sqrt_2pi = M_2_SQRTPI/M_SQRT2;
          return 0.5f*(1 + std::erf(z/M_SQRT2)) + (std::exp(-math::sq(z)*0.5f)*z)*c_1_sqrt_2pi;
        }
        case PhiType::SELU: return l*phi_diff(z, PhiType::ELU, a, k, l);
      }
    }
  
    template<typename Cont>
    Cont softmax(const Cont& c, float p = 1)
    {
      Cont ret = c;
      Cont ec = c;
      if (p == 1)
        for (auto& v : ec)
          v = std::exp(v);
      else
        for (auto& v : ec)
          v = std::exp(std::pow(v, p));
      auto ecs = stlutils::sum(ec);
      auto N = c.size();
      for (size_t i = 0; i < N; ++i)
        ret[i] = ec[i]/ecs;
      return ret;
    }
  
    class Input
    {
      float perceptron_signal = 0.f;
      const float* dendrite_output = nullptr;
      bool set = false;
      
    public:
      Input() = default;
      Input(const float signal) : perceptron_signal(signal), set(true) {}
      Input(const float* output) : dendrite_output(output), set(true) {}
      
      std::optional<float> get() const
      {
        if (!set)
          return {};
        if (dendrite_output != nullptr)
          return *dendrite_output;
        return perceptron_signal;
      }
    };
    
    class Neuron
    {
      const size_t Nw = 0;
      std::vector<Input> inputs;
      std::vector<float> weights;
      float bias = 0.f;
      float z = 0.f;
      PhiType phi_type;
      float phi_param_a = 1.f;
      float phi_param_k = 1.f;
      float phi_param_l = 1.1f;
      float y = 0.f;
      
      std::vector<float> w_diff_prev;
      float b_diff_prev = 0.f;
      
    public:
      Neuron(const std::vector<float>& w, float b, PhiType af_type)
        : Nw(w.size()), weights(w), bias(b), phi_type(af_type)
      {
        w_diff_prev.assign(Nw, 0);
      }
      
      void set_inputs(const std::vector<Input>& x)
      {
        assert(x.size() == Nw);
        inputs = x;
      }
      
      void set_phi_params(float a, float k, float l)
      {
        phi_param_a = a;
        phi_param_k = k;
        phi_param_l = l;
      }
      
      float update_forward()
      {
        std::vector<float> x, w;
        for (int i = 0; i < Nw; ++i)
        {
          auto ival = inputs[i].get();
          if (ival.has_value())
          {
            x.emplace_back(ival.value());
            w.emplace_back(weights[i]);
          }
        }
        z = stlutils::dot(x, w);
        z += bias;
        y = phi(z, phi_type, phi_param_a, phi_param_k, phi_param_l);
        return y;
      }
      
      // Back-prop
      // y_trg : target output.
      // eta : learning rate (0.1).
      // mu : momentum term (0.5).
      // r : random term for simulated annealing-ish behaviour (0).
      // diff = eta * (-grad + mu * diff_prev + r)
      // Returns gradient.
      std::vector<float> update_backward(float y_trg, float eta = 0.1f, float mu = 0.5f, float r = 0.f)
      {
        // dC/dw1 = dC/df * df/dz * dz/dw
        auto err_diff = -(y_trg - y);
        auto dC_df = err_diff;
        auto df_dz = phi_diff(z, phi_type, phi_param_a, phi_param_k, phi_param_l);
        std::vector<float> dz_dw(Nw); // z = w0*x0 + w1*x1 + b => dz/dw0 = x0, dz/dw1 = x1, dz/db = 1.
        for (size_t i = 0; i < Nw; ++i)
          dz_dw[i] = inputs[i].get().value_or(0);
        auto dz_db = 1.f;
        auto dC_dz = dC_df * df_dz;
        auto dC_dw = stlutils::mult_scalar(dz_dw, dC_dz);
        auto dC_db = dC_dz * dz_db;
        
        auto w_diff = stlutils::mult_scalar(dC_dw, -1);
        w_diff = stlutils::add(w_diff, stlutils::mult_scalar(w_diff_prev, mu));
        w_diff = stlutils::add_scalar(w_diff, r);
        w_diff = stlutils::mult_scalar(w_diff, eta);
        auto b_diff = eta * (-dC_db + mu * b_diff_prev + r);
        
        weights = stlutils::add(weights, w_diff);
        bias += b_diff;
        
        w_diff_prev = w_diff;
        b_diff_prev = b_diff;
        
        return dC_dw;
      }
      
      // Forward-prop followed by a back-prop.
      // y_trg : target output.
      // eta : learning rate (0.1).
      // mu : momentum term (0.5).
      // r : random term for simulated annealing-ish behaviour (0).
      // diff = eta * (-grad + mu * diff_prev + r)
      // Returns gradient.
      std::vector<float> train(float y_trg, float eta = 0.1f, float mu = 0.5f, float r = 0.f)
      {
        update_forward();
        return update_backward(y_trg, eta, mu, r);
      }
      
      const float* output() const { return &y; }
    };
  
    //
  
    class NeuralLayer
    {
      const size_t Ni = 0;
      const size_t No = 0;
      std::vector<std::unique_ptr<Neuron>> neurons;
      
    public:
      NeuralLayer(const std::vector<std::vector<float>>& w,
                  const std::vector<float>& b,
                  PhiType af_type)
        : No(w.size()), Ni(w[0].size())
      {
        for (size_t n_idx = 0; n_idx < No; ++n_idx)
          neurons.emplace_back(std::make_unique<Neuron>(w[n_idx], b[n_idx], af_type));
      }
      
      void set_inputs(const std::vector<Input>& x)
      {
        for (auto& n : neurons)
          n->set_inputs(x);
      }
      
      void set_phi_params(float a, float k, float l)
      {
        for (auto& n : neurons)
          n->set_phi_params(a, k, l);
      }
      
      void update_forward()
      {
        for (auto& n : neurons)
          n->update_forward();
      }
      
      // Back-prop
      // y_trg : target output.
      // eta : learning rate (0.1).
      // mu : momentum term (0.5).
      // r : random term for simulated annealing-ish behaviour (0).
      // diff = eta * (-grad + mu * diff_prev + r)
      // Returns gradient.
      std::vector<std::vector<float>> update_backward(const std::vector<float>& y_trg,
                                                      float eta = 0.1f, float mu = 0.5f, float r = 0.f)
      {
        assert(y_trg.size() == No);
        std::vector<std::vector<float>> grad(No);
        for (size_t n_idx = 0; n_idx < No; ++n_idx)
          grad[n_idx] = neurons[n_idx]->update_backward(y_trg[n_idx], eta, mu, r);
        return grad;
      }
      
      // Forward-prop followed by a back-prop.
      // y_trg : target output.
      // eta : learning rate (0.1).
      // mu : momentum term (0.5).
      // r : random term for simulated annealing-ish behaviour (0).
      // diff = eta * (-grad + mu * diff_prev + r)
      // Returns gradient.
      std::vector<std::vector<float>> train(const std::vector<float>& y_trg,
                                            float eta = 0.1f, float mu = 0.5f, float r = 0.f)
      {
        update_forward();
        return update_backward(y_trg, eta, mu, r);
      }
      
      const std::vector<Input> output() const
      {
        std::vector<Input> ret(No);
        for (size_t n_idx = 0; n_idx < No; ++n_idx)
          ret[n_idx] = neurons[n_idx]->output();
        return ret;
      }
      
      const Neuron* operator[](size_t n_idx) const
      {
        assert(n_idx < No);
        return neurons[n_idx].get();
      }
      
      size_t num_inputs() const { return Ni; }
      size_t num_outputs() const { return No; }
    };
  
    //
  
    class NeuralNetwork
    {
      const size_t Nl = 0;
      std::vector<std::unique_ptr<NeuralLayer>> layers;
    
    public:
      NeuralNetwork(const std::vector<std::vector<std::vector<float>>>& w,
                  const std::vector<std::vector<float>>& b,
                  const std::vector<PhiType>& af_type)
        : Nl(w.size())
      {
        for (size_t l_idx = 0; l_idx < Nl; ++l_idx)
          layers.emplace_back(std::make_unique<NeuralLayer>(w[l_idx], b[l_idx], af_type[l_idx]));
        for (size_t l_idx = 1; l_idx < Nl; ++l_idx)
          layers[l_idx]->set_inputs(layers[l_idx - 1]->output());
      }
    
      void set_inputs(const std::vector<Input>& x)
      {
        layers.front()->set_inputs(x);
      }
      
      void set_phi_params(float a, float k, float l)
      {
        for (auto& ll : layers)
          ll->set_phi_params(a, k, l);
      }
      
      void update_forward()
      {
        for (auto& l : layers)
          l->update_forward();
      }
      
      // Back-prop
      // y_trg : target output.
      // eta : learning rate (0.1).
      // mu : momentum term (0.5).
      // r : random term for simulated annealing-ish behaviour (0).
      // diff = eta * (-grad + mu * diff_prev + r)
      // Returns gradient.
      std::vector<std::vector<float>> update_backward(const std::vector<float>& y_trg,
                                                      float eta = 0.1f, float mu = 0.5f, float r = 0.f)
      {
        size_t No = num_outputs();
        assert(y_trg.size() == No);
        auto* l = layers.back().get();
        auto grad = l->update_backward(y_trg, eta, mu, r);
        for (int l_idx = static_cast<int>(Nl) - 2; l_idx >= 0; --l_idx)
        {
          size_t Ni = l->num_inputs();
          size_t No = l->num_outputs();
          std::vector<float> grad_flat(Ni, 0);
          l = layers[l_idx].get();
          for (size_t o_idx = 0; o_idx < No; ++o_idx)
            for (size_t i_idx = 0; i_idx < Ni; ++i_idx)
              grad_flat[i_idx] += grad[o_idx][i_idx];
          grad = l->update_backward(grad_flat, eta, mu, r);
        }
        return grad;
      }
      
      // Forward-prop followed by a back-prop.
      // y_trg : target output.
      // eta : learning rate (0.1).
      // mu : momentum term (0.5).
      // r : random term for simulated annealing-ish behaviour (0).
      // diff = eta * (-grad + mu * diff_prev + r)
      // Returns gradient.
      std::vector<std::vector<float>> train(const std::vector<float>& y_trg,
                                            float eta = 0.1f, float mu = 0.5f, float r = 0.f)
      {
        update_forward();
        return update_backward(y_trg, eta, mu, r);
      }
      
      const std::vector<Input> output() const
      {
        return layers.back()->output();
      }
      
      const NeuralLayer* operator[](size_t l_idx) const
      {
        assert(l_idx < Nl);
        return layers[l_idx].get();
      }
      
      size_t num_inputs() const { return layers.front()->num_inputs(); }
      size_t num_outputs() const { return layers.back()->num_outputs(); }
    };
  
  }

}