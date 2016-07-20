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
v_0.SetName("if then else test");
v_0.SetInternal(false);
v_0.SetExpr(" if root.Input_value_1<root.Input_value_2 then root.Static_Input_value_1 else  root.Static_Input_value_2 end ");
v_0.SetExpectsFile(false);
v_0.SetInitOnly(false);
root.AddVariable(&v_0);

v_1.SetID("v_1");
v_1.SetName("Input_value_1");
v_1.SetInternal(false);
v_1.SetExpr("1.0");
v_1.SetExpectsFile(false);
v_1.SetInitOnly(true);
root.AddVariable(&v_1);

v_2.SetID("v_2");
v_2.SetName("Input_value_2");
v_2.SetInternal(false);
v_2.SetExpr("0.0");
v_2.SetExpectsFile(false);
v_2.SetInitOnly(true);
root.AddVariable(&v_2);

v_3.SetID("v_3");
v_3.SetName("Static_Input_value 1");
v_3.SetInternal(false);
v_3.SetExpr("5.0");
v_3.SetExpectsFile(false);
v_3.SetInitOnly(true);
root.AddVariable(&v_3);

v_4.SetID("v_4");
v_4.SetName("Static_Input_value_2");
v_4.SetInternal(false);
v_4.SetExpr("3.0");
v_4.SetExpectsFile(false);
v_4.SetInitOnly(true);
root.AddVariable(&v_4);

v_5.SetID("v_5");
v_5.SetName("xidz test");
v_5.SetInternal(false);
v_5.SetExpr("if abs(root.Input_value_2)<1e-6 then  root.Static_Input_value_1 else (root.Input_value_1)/(root.Input_value_2) end");
v_5.SetExpectsFile(false);
v_5.SetInitOnly(false);
root.AddVariable(&v_5);

v_6.SetID("v_6");
v_6.SetName("zidz test");
v_6.SetInternal(false);
v_6.SetExpr("if abs( root.Input_value_2)<1e-6 then 0.0 else (root.Input_value_1)/( root.Input_value_2) end");
v_6.SetExpectsFile(false);
v_6.SetInitOnly(false);
root.AddVariable(&v_6);

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
root.JoinAliases();
return &root;
}

void TestModel::DoInit(const OME_SCALAR & TestModelTime, BaseManager* TestModelBm,ISpatialDataProvider* TestModelSDP)
{
using namespace TestModelDefs;
using namespace OMECFuncs;
//=-=-=-=-=-=-=-=-=-= v_1 (Input_value_1)
v_1.Reset(TestModelBm);
v_1=(OME_SCALAR)1.0;
v_1.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_2 (Input_value_2)
v_2.Reset(TestModelBm);
v_2=(OME_SCALAR)0.0;
v_2.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_6 (zidz_test)
v_6.Reset(TestModelBm);
v_6=(OME_SCALAR)(OMECFuncs::abs(v_2)<1e-6 ? (OME_SCALAR)0.0 : (OME_SCALAR)(v_1)/(v_2));
v_6.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_3 (Static_Input_value_1)
v_3.Reset(TestModelBm);
v_3=(OME_SCALAR)5.0;
v_3.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_5 (xidz_test)
v_5.Reset(TestModelBm);
v_5=(OME_SCALAR)(OMECFuncs::abs(v_2)<1e-6 ? (OME_SCALAR)v_3 : (OME_SCALAR)(v_1)/(v_2));
v_5.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_4 (Static_Input_value_2)
v_4.Reset(TestModelBm);
v_4=(OME_SCALAR)3.0;
v_4.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= v_0 (if_then_else_test)
v_0.Reset(TestModelBm);
v_0=(OME_SCALAR)(v_1<v_2 ? (OME_SCALAR)v_3 : (OME_SCALAR)v_4);
v_0.BatchInitialize(TestModelTime,TestModelBm);
AdjustTempPool();
}

void TestModel::DoUpdate(const OME_SCALAR & TestModelTime, BaseManager* TestModelBm,ISpatialDataProvider* TestModelSDP)
{
using namespace TestModelDefs;
using namespace OMECFuncs;
//=-=-=-=-=-=-=-=-=-= v_6 (zidz_test)
v_6=(OME_SCALAR)(OMECFuncs::abs(v_2)<1e-6 ? (OME_SCALAR)0.0 : (OME_SCALAR)(v_1)/(v_2));

//=-=-=-=-=-=-=-=-=-= v_5 (xidz_test)
v_5=(OME_SCALAR)(OMECFuncs::abs(v_2)<1e-6 ? (OME_SCALAR)v_3 : (OME_SCALAR)(v_1)/(v_2));

//=-=-=-=-=-=-=-=-=-= v_0 (if_then_else_test)
v_0=(OME_SCALAR)(v_1<v_2 ? (OME_SCALAR)v_3 : (OME_SCALAR)v_4);
AdjustTempPool();
}

