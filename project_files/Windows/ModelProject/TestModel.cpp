#include "TestModel.h"
#include <map>
#include "OMEDefines.h"
#include "SubIndexVal.h"
#include "ValueArchive.h"
namespace TestModelDefs
{
Model& root=*((Model*)BaseManager::GenModelObject(OME_MODEL));
Variable& v_0=*((Variable*)BaseManager::GenModelObject(OME_VARIABLE));
Variable& v_1=*((Variable*)BaseManager::GenModelObject(OME_VARIABLE));
Variable& v_2=*((Variable*)BaseManager::GenModelObject(OME_VARIABLE));
Variable& v_3=*((Variable*)BaseManager::GenModelObject(OME_VARIABLE));
Variable& v_4=*((Variable*)BaseManager::GenModelObject(OME_VARIABLE));
Variable& v_5=*((Variable*)BaseManager::GenModelObject(OME_VARIABLE));
Variable& v_6=*((Variable*)BaseManager::GenModelObject(OME_VARIABLE));
Variable& v_7=*((Variable*)BaseManager::GenModelObject(OME_VARIABLE));
Variable& v_8=*((Variable*)BaseManager::GenModelObject(OME_VARIABLE));
Variable& v_9=*((Variable*)BaseManager::GenModelObject(OME_VARIABLE));
Variable& v_10=*((Variable*)BaseManager::GenModelObject(OME_VARIABLE));
Variable& v_11=*((Variable*)BaseManager::GenModelObject(OME_VARIABLE));
Variable& v_12=*((Variable*)BaseManager::GenModelObject(OME_VARIABLE));
Variable& v_13=*((Variable*)BaseManager::GenModelObject(OME_VARIABLE));
Variable& v_14=*((Variable*)BaseManager::GenModelObject(OME_VARIABLE));
Variable& v_15=*((Variable*)BaseManager::GenModelObject(OME_VARIABLE));
Variable& v_16=*((Variable*)BaseManager::GenModelObject(OME_VARIABLE));


};
Model* TestModel::LoadModel(BaseManager* TestModelBm)
{
using namespace TestModelDefs;
using namespace OMECFuncs;
root.SetID("root");
root.SetName("root");
root.SetInternal(false);
root.SetInitNumInstances(1);
root.SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
v_0.SetID("v_0");
v_0.SetName("Input 1");
v_0.SetInternal(false);
v_0.SetExpr("2.0");
v_0.SetExpectsFile(false);
v_0.SetInitOnly(true);
root.AddVariable(&v_0);

v_1.SetID("v_1");
v_1.SetName("Input 2");
v_1.SetInternal(false);
v_1.SetExpr("2.0");
v_1.SetExpectsFile(false);
v_1.SetInitOnly(true);
root.AddVariable(&v_1);

v_2.SetID("v_2");
v_2.SetName("Input 3");
v_2.SetInternal(false);
v_2.SetExpr("1.0");
v_2.SetExpectsFile(false);
v_2.SetInitOnly(true);
root.AddVariable(&v_2);

v_3.SetID("v_3");
v_3.SetName("Input max");
v_3.SetInternal(false);
v_3.SetExpr("2.0");
v_3.SetExpectsFile(false);
v_3.SetInitOnly(true);
root.AddVariable(&v_3);

v_4.SetID("v_4");
v_4.SetName("Input min");
v_4.SetInternal(false);
v_4.SetExpr("0.0");
v_4.SetExpectsFile(false);
v_4.SetInitOnly(true);
root.AddVariable(&v_4);

v_5.SetID("v_5");
v_5.SetName("Input shift");
v_5.SetInternal(false);
v_5.SetExpr("0.0");
v_5.SetExpectsFile(false);
v_5.SetInitOnly(true);
root.AddVariable(&v_5);

v_6.SetID("v_6");
v_6.SetName("Input stretch");
v_6.SetInternal(false);
v_6.SetExpr("1.0");
v_6.SetExpectsFile(false);
v_6.SetInitOnly(true);
root.AddVariable(&v_6);

v_7.SetID("v_7");
v_7.SetName("random beta test");
v_7.SetInternal(false);
v_7.SetExpr("beta_ran(root.Input_1,root.Input_2,root.Input_min,root.Input_max,root.Input_shift,root.Input_stretch)");
v_7.SetExpectsFile(false);
v_7.SetInitOnly(false);
root.AddVariable(&v_7);

v_8.SetID("v_8");
v_8.SetName("random binomial test");
v_8.SetInternal(false);
v_8.SetExpr("binome(root.Input_1,root.Input_2,root.Input_min,root.Input_max,root.Input_shift,root.Input_stretch)");
v_8.SetExpectsFile(false);
v_8.SetInitOnly(false);
root.AddVariable(&v_8);

v_9.SetID("v_9");
v_9.SetName("random exponential test");
v_9.SetInternal(false);
v_9.SetExpr("exponent_ran(root.Input_min,root.Input_max,root.Input_shift,root.Input_stretch)");
v_9.SetExpectsFile(false);
v_9.SetInitOnly(false);
root.AddVariable(&v_9);

v_10.SetID("v_10");
v_10.SetName("random gamma test");
v_10.SetInternal(false);
v_10.SetExpr("gamma_ran(root.Input_1,root.Input_stretch,root.Input_min,root.Input_max,root.Input_shift)");
v_10.SetExpectsFile(false);
v_10.SetInitOnly(false);
root.AddVariable(&v_10);

v_11.SetID("v_11");
v_11.SetName("random negative binomial test");
v_11.SetInternal(false);
v_11.SetExpr("negBinome_ran(root.Input_1,root.Input_2,root.Input_min,root.Input_max,root.Input_shift,root.Input_stretch)");
v_11.SetExpectsFile(false);
v_11.SetInitOnly(false);
root.AddVariable(&v_11);

v_12.SetID("v_12");
v_12.SetName("random normal test");
v_12.SetInternal(false);
v_12.SetExpr("gaussian_var(root.Input_1,root.Input_2,root.Input_min,root.Input_max)");
v_12.SetExpectsFile(false);
v_12.SetInitOnly(false);
root.AddVariable(&v_12);

v_13.SetID("v_13");
v_13.SetName("random poisson test");
v_13.SetInternal(false);
v_13.SetExpr("poidev(root.Input_1,root.Input_min,root.Input_max,root.Input_shift,root.Input_stretch)");
v_13.SetExpectsFile(false);
v_13.SetInitOnly(false);
root.AddVariable(&v_13);

v_14.SetID("v_14");
v_14.SetName("random triangular test");
v_14.SetInternal(false);
v_14.SetExpr("triangle_ran(root.Input_1,root.Input_3,root.Input_2,root.Input_min,root.Input_max)");
v_14.SetExpectsFile(false);
v_14.SetInitOnly(false);
root.AddVariable(&v_14);

v_15.SetID("v_15");
v_15.SetName("random uniform test");
v_15.SetInternal(false);
v_15.SetExpr("rand_var(root.Input_min,root.Input_max)");
v_15.SetExpectsFile(false);
v_15.SetInitOnly(false);
root.AddVariable(&v_15);

v_16.SetID("v_16");
v_16.SetName("random weibull test");
v_16.SetInternal(false);
v_16.SetExpr("weib_ran(root.Input_1,root.Input_stretch,root.Input_min,root.Input_max,root.Input_shift)");
v_16.SetExpectsFile(false);
v_16.SetInitOnly(false);
root.AddVariable(&v_16);

root.JoinLinks(true,true);
//Begin Lookup table


Register(&root);
Register(&v_0);
Register(&v_1);
Register(&v_2);
Register(&v_3);
Register(&v_4);
Register(&v_5);
Register(&v_6);
Register(&v_7);
Register(&v_8);
Register(&v_9);
Register(&v_10);
Register(&v_11);
Register(&v_12);
Register(&v_13);
Register(&v_14);
Register(&v_15);
Register(&v_16);
root.JoinAliases();
return &root;
}

void TestModel::DoInit(const OME_SCALAR & TestModelTime, BaseManager* TestModelBm,ISpatialDataProvider* TestModelSDP)
{
using namespace TestModelDefs;
using namespace OMECFuncs;
//=-=-=-=-=-=-=-=-=-= v_0 (Input_1)
v_0.Reset(TestModelBm);
v_0=(OME_SCALAR)2.0;
v_0.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_1 (Input_2)
v_1.Reset(TestModelBm);
v_1=(OME_SCALAR)2.0;
v_1.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_2 (Input_3)
v_2.Reset(TestModelBm);
v_2=(OME_SCALAR)1.0;
v_2.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_3 (Input_max)
v_3.Reset(TestModelBm);
v_3=(OME_SCALAR)2.0;
v_3.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_4 (Input_min)
v_4.Reset(TestModelBm);
v_4=(OME_SCALAR)0.0;
v_4.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_14 (random_triangular_test)
v_14.Reset(TestModelBm);
v_14=(OME_SCALAR)triangle_ran((v_0),(v_2),(v_1),(v_4),(v_3));
v_14.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_12 (random_normal_test)
v_12.Reset(TestModelBm);
v_12=(OME_SCALAR)gaussian_var((v_0),(v_1),(v_4),(v_3));
v_12.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_5 (Input_shift)
v_5.Reset(TestModelBm);
v_5=(OME_SCALAR)0.0;
v_5.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_6 (Input_stretch)
v_6.Reset(TestModelBm);
v_6=(OME_SCALAR)1.0;
v_6.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_8 (random_binomial_test)
v_8.Reset(TestModelBm);
v_8=(OME_SCALAR)binome((v_0),(v_1),(v_4),(v_3),(v_5),(v_6));
v_8.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_7 (random_beta_test)
v_7.Reset(TestModelBm);
v_7=(OME_SCALAR)beta_ran((v_0),(v_1),(v_4),(v_3),(v_5),(v_6));
v_7.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_16 (random_weibull_test)
v_16.Reset(TestModelBm);
v_16=(OME_SCALAR)weib_ran((v_0),(v_6),(v_4),(v_3),(v_5));
v_16.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_13 (random_poisson_test)
v_13.Reset(TestModelBm);
v_13=(OME_SCALAR)poidev((v_0),(v_4),(v_3),(v_5),(v_6));
v_13.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_11 (random_negative_binomial_test)
v_11.Reset(TestModelBm);
v_11=(OME_SCALAR)negBinome_ran((v_0),(v_1),(v_4),(v_3),(v_5),(v_6));
v_11.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_10 (random_gamma_test)
v_10.Reset(TestModelBm);
v_10=(OME_SCALAR)gamma_ran((v_0),(v_6),(v_4),(v_3),(v_5));
v_10.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_15 (random_uniform_test)
v_15.Reset(TestModelBm);
v_15=(OME_SCALAR)rand_var((v_4),(v_3));
v_15.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_9 (random_exponential_test)
v_9.Reset(TestModelBm);
v_9=(OME_SCALAR)exponent_ran((v_4),(v_3),(v_5),(v_6));
v_9.BatchInitialize(TestModelTime,TestModelBm);
AdjustTempPool();
}

void TestModel::DoUpdate(const OME_SCALAR & TestModelTime, BaseManager* TestModelBm,ISpatialDataProvider* TestModelSDP)
{
using namespace TestModelDefs;
using namespace OMECFuncs;
//=-=-=-=-=-=-=-=-=-= v_15 (random_uniform_test)
v_15=(OME_SCALAR)rand_var((v_4),(v_3));

//=-=-=-=-=-=-=-=-=-= v_14 (random_triangular_test)
v_14=(OME_SCALAR)triangle_ran((v_0),(v_2),(v_1),(v_4),(v_3));

//=-=-=-=-=-=-=-=-=-= v_12 (random_normal_test)
v_12=(OME_SCALAR)gaussian_var((v_0),(v_1),(v_4),(v_3));

//=-=-=-=-=-=-=-=-=-= v_9 (random_exponential_test)
v_9=(OME_SCALAR)exponent_ran((v_4),(v_3),(v_5),(v_6));

//=-=-=-=-=-=-=-=-=-= v_8 (random_binomial_test)
v_8=(OME_SCALAR)binome((v_0),(v_1),(v_4),(v_3),(v_5),(v_6));

//=-=-=-=-=-=-=-=-=-= v_7 (random_beta_test)
v_7=(OME_SCALAR)beta_ran((v_0),(v_1),(v_4),(v_3),(v_5),(v_6));

//=-=-=-=-=-=-=-=-=-= v_16 (random_weibull_test)
v_16=(OME_SCALAR)weib_ran((v_0),(v_6),(v_4),(v_3),(v_5));

//=-=-=-=-=-=-=-=-=-= v_13 (random_poisson_test)
v_13=(OME_SCALAR)poidev((v_0),(v_4),(v_3),(v_5),(v_6));

//=-=-=-=-=-=-=-=-=-= v_11 (random_negative_binomial_test)
v_11=(OME_SCALAR)negBinome_ran((v_0),(v_1),(v_4),(v_3),(v_5),(v_6));

//=-=-=-=-=-=-=-=-=-= v_10 (random_gamma_test)
v_10=(OME_SCALAR)gamma_ran((v_0),(v_6),(v_4),(v_3),(v_5));
AdjustTempPool();
}

