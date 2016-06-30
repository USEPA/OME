#include "TestModel.h"
#include <map>
#include "OMEDefines.h"
#include "SubIndexVal.h"
#include "ValueArchive.h"
#define OME_MAX_NEIGHBORS 0//derived from coverage at code generation time. Update if coverage changes
namespace TestModelDefs
{
Model* gp_root=NULL;
Variable* gp_node00059=NULL;
Variable* gp_node00062=NULL;
Variable* gp_node00228=NULL;
Variable* gp_node03602=NULL;
Variable* gp_node03603=NULL;
Variable* gp_node03604=NULL;
Variable* gp_node03605=NULL;
Variable* gp_node03607=NULL;
Variable* gp_node03612=NULL;
Variable* gp_node03614=NULL;
Variable* gp_node03617=NULL;
Variable* gp_node03619=NULL;
Variable* gp_node03620=NULL;
Variable* gp_node03626=NULL;
Variable* gp_node03628=NULL;
Variable* gp_node03630=NULL;
Variable* gp_node03632=NULL;
Variable* gp_node03634=NULL;
Variable* gp_node03638=NULL;
Variable* gp_node03640=NULL;
Variable* gp_node03651=NULL;
Variable* gp_node03653=NULL;
Variable* gp_node03668=NULL;
Variable* gp_node03669=NULL;
Variable* gp_node03671=NULL;
Variable* gp_node03672=NULL;
Variable* gp_node03680=NULL;
Variable* gp_node03682=NULL;
Variable* gp_node03683=NULL;
Variable* gp_node03685=NULL;
Variable* gp_node03693=NULL;
Variable* gp_node03695=NULL;
Variable* gp_node03697=NULL;
Variable* gp_node03702=NULL;
Variable* gp_node03704=NULL;
Variable* gp_node03707=NULL;
Variable* gp_node03709=NULL;
Variable* gp_node03712=NULL;
Variable* gp_node03718=NULL;
Variable* gp_node03720=NULL;
Variable* gp_node03723=NULL;
Variable* gp_node03725=NULL;
VarArray* gp_node03594=NULL;
VarArray* gp_node03596=NULL;
VarArray* gp_node03598=NULL;
VarArray* gp_node03600=NULL;
VarArray* gp_node03641=NULL;
VarArray* gp_node03656=NULL;
VarArray* gp_node03657=NULL;
VarArray* gp_node03674=NULL;
VarArray* gp_node03677=NULL;
VarArray* gp_node03696=NULL;
VarArray* gp_node03699=NULL;
VarArray* gp_node03706=NULL;
VarArray* gp_node03728=NULL;
TimeSeriesVar* gp_node00060=NULL;
TimeSeriesVar* gp_node03637=NULL;
TimeSeriesVar* gp_node03648=NULL;
TimeSeriesVar* gp_node03694=NULL;
TimeSeriesVar* gp_node03722=NULL;
StateVar* gp_node03622=NULL;
StateVar* gp_node03646=NULL;
StateVar* gp_node03663=NULL;
StateVar* gp_node03687=NULL;
Flow* gp_arc03874=NULL;
Flow* gp_arc04241=NULL;
Flow* gp_arc04229=NULL;
Flow* gp_arc04232=NULL;
Flow* gp_arc04234=NULL;
Flow* gp_arc04266=NULL;
Flow* gp_arc04268=NULL;
Model* gp_node03593=NULL;
Model* gp_node03732=NULL;
Variable* gp_node03734=NULL;
Model* gp_node03595=NULL;
Model* gp_node03739=NULL;
Model* gp_node03744=NULL;
Variable* gp_node03746=NULL;
Model* gp_node03740=NULL;
Model* gp_node03749=NULL;
Variable* gp_node03751=NULL;
Model* gp_node03741=NULL;
Model* gp_node03753=NULL;
Variable* gp_node03755=NULL;
Model* gp_node03743=NULL;
Model* gp_node03756=NULL;
Variable* gp_node03759=NULL;
Model* gp_node03606=NULL;
Model* gp_node03760=NULL;
Variable* gp_node03765=NULL;
Variable* gp_node03766=NULL;
Variable* gp_node03767=NULL;
Variable* gp_node03768=NULL;
Model* gp_node03608=NULL;
Model* gp_node03776=NULL;
Variable* gp_node03781=NULL;
Variable* gp_node03783=NULL;
Variable* gp_node03784=NULL;
Variable* gp_node03786=NULL;
Model* gp_node03611=NULL;
Variable* gp_node03788=NULL;
Variable* gp_node03790=NULL;
Variable* gp_node03792=NULL;
Variable* gp_node03794=NULL;
Variable* gp_node03797=NULL;
Variable* gp_node03800=NULL;
Model* gp_node03627=NULL;
Variable* gp_node03804=NULL;
Variable* gp_node03809=NULL;
Variable* gp_node03812=NULL;
Variable* gp_node03814=NULL;
Variable* gp_node03817=NULL;
Variable* gp_node03820=NULL;
Variable* gp_node03821=NULL;
Variable* gp_node03823=NULL;
Variable* gp_node03973=NULL;
Model* gp_node03642=NULL;
Variable* gp_node03825=NULL;
Variable* gp_node03826=NULL;
Variable* gp_node03828=NULL;
Variable* gp_node03830=NULL;
Variable* gp_node03834=NULL;
Variable* gp_node03836=NULL;
Variable* gp_node03841=NULL;
Variable* gp_node03844=NULL;
Variable* gp_node03848=NULL;
Variable* gp_node03851=NULL;
Variable* gp_node03853=NULL;
Variable* gp_node03856=NULL;
Variable* gp_node03862=NULL;
Variable* gp_node03863=NULL;
Variable* gp_node03869=NULL;
Variable* gp_node03871=NULL;
Variable* gp_node03872=NULL;
Variable* gp_node03877=NULL;
Variable* gp_node03880=NULL;
Variable* gp_node03883=NULL;
Variable* gp_node03893=NULL;
Model* gp_node03847=NULL;
Variable* gp_node03904=NULL;
StateVar* gp_node03896=NULL;
Flow* gp_arc03930=NULL;
Model* gp_node03644=NULL;
Variable* gp_node00073=NULL;
Variable* gp_node00074=NULL;
Variable* gp_node00077=NULL;
Variable* gp_node00079=NULL;
Variable* gp_node00230=NULL;
Variable* gp_node03927=NULL;
Variable* gp_node03929=NULL;
Variable* gp_node03934=NULL;
Variable* gp_node03939=NULL;
Variable* gp_node03950=NULL;
Variable* gp_node03952=NULL;
Variable* gp_node03954=NULL;
Variable* gp_node03958=NULL;
Variable* gp_node03963=NULL;
Variable* gp_node03965=NULL;
Variable* gp_node03970=NULL;
Variable* gp_node03971=NULL;
Variable* gp_node03975=NULL;
Variable* gp_node03977=NULL;
Variable* gp_node03979=NULL;
Variable* gp_node03983=NULL;
Variable* gp_node03990=NULL;
Variable* gp_node04000=NULL;
Variable* gp_node04004=NULL;
VarArray* gp_node03961=NULL;
StateVar* gp_node03613=NULL;
Flow* gp_arc04227=NULL;
Flow* gp_arc04360=NULL;
Model* gp_node00017=NULL;
Variable* gp_node00021=NULL;
Variable* gp_node00034=NULL;
Variable* gp_node00036=NULL;
Variable* gp_node00045=NULL;
Variable* gp_node00046=NULL;
Variable* gp_node00047=NULL;
Variable* gp_node00048=NULL;
StateVar* gp_node00024=NULL;
StateVar* gp_node00030=NULL;
StateVar* gp_node00052=NULL;
Flow* gp_arc01510=NULL;
Flow* gp_arc01203=NULL;
Flow* gp_arc01406=NULL;
Flow* gp_arc01511=NULL;
Flow* gp_arc01738=NULL;
Flow* gp_arc01739=NULL;
Model* gp_node03967=NULL;
Variable* gp_node04013=NULL;
Variable* gp_node04015=NULL;
Variable* gp_node04023=NULL;
Variable* gp_node04024=NULL;
VarArray* gp_node04019=NULL;
VarArray* gp_node04020=NULL;
VarArray* gp_node04021=NULL;
Model* gp_node03988=NULL;
Variable* gp_node04030=NULL;
Variable* gp_node04034=NULL;
Variable* gp_node04039=NULL;
Variable* gp_node04041=NULL;
Variable* gp_node04046=NULL;
Variable* gp_node04052=NULL;
Variable* gp_node04054=NULL;
Variable* gp_node04056=NULL;
Variable* gp_node04060=NULL;
Variable* gp_node04064=NULL;
StateVar* gp_node04043=NULL;
Model* gp_node04002=NULL;
Variable* gp_node04071=NULL;
Variable* gp_node04080=NULL;
StateVar* gp_node04081=NULL;
Flow* gp_arc04201=NULL;
Flow* gp_arc04194=NULL;
Model* gp_node03655=NULL;
Model* gp_node04090=NULL;
VarArray* gp_node04091=NULL;
Model* gp_node03700=NULL;
Variable* gp_node04093=NULL;
Variable* gp_node04094=NULL;
Variable* gp_node04108=NULL;
Variable* gp_node04112=NULL;
Variable* gp_node04113=NULL;
Spawner* gp_node04105=NULL;
Model* gp_node03711=NULL;
Variable* gp_node04115=NULL;
Variable* gp_node04117=NULL;
Variable* gp_node04118=NULL;
Variable* gp_node04119=NULL;
Variable* gp_node04121=NULL;
Variable* gp_node04122=NULL;
Variable* gp_node04123=NULL;
Variable* gp_node04124=NULL;
Variable* gp_node04130=NULL;
Model* gp_node03717=NULL;
Variable* gp_node04135=NULL;
Variable* gp_node04138=NULL;
Variable* gp_node04140=NULL;
Variable* gp_node04147=NULL;
Variable* gp_node04150=NULL;
VarArray* gp_node04142=NULL;
Spawner* gp_node04144=NULL;
Model* gp_node03730=NULL;
Variable* gp_node04157=NULL;
Variable* gp_node04159=NULL;
Variable* gp_node04163=NULL;
Variable* gp_node04165=NULL;
Variable* gp_node04169=NULL;
Variable* gp_node04171=NULL;
Spawner* gp_node04153=NULL;
Model* gp_node03731=NULL;
Variable* gp_node04173=NULL;
Variable* gp_node04174=NULL;


};
Model* TestModel::LoadModel(BaseManager* TestModelBm)
{
using namespace TestModelDefs;
using namespace OMECFuncs;
gp_root=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_root->SetID("root");
gp_root->SetName("AustinModel-determ");
gp_root->SetInternal(false);
gp_root->SetInitNumInstances(1);
gp_root->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
EvalTable::DataArray root_tableData;
IndVec root_tableDims;
root_tableData.clear();
root_tableData.push_back(3.4);
root_tableData.push_back(-0.5);
root_tableData.push_back(-2.7);
root_tableData.push_back(-1.6);
root_tableData.push_back(3.9);
root_tableData.push_back(10.4);
root_tableData.push_back(14.9);
root_tableData.push_back(18.1);
root_tableData.push_back(20.3);
root_tableData.push_back(22.3);
root_tableData.push_back(24.3);
root_tableData.push_back(26);
root_tableData.push_back(27.4);
root_tableData.push_back(28.4);
root_tableData.push_back(29);
root_tableData.push_back(29.3);
root_tableData.push_back(29);
root_tableData.push_back(28.4);
root_tableData.push_back(27.3);
root_tableData.push_back(25.8);
root_tableData.push_back(24);
root_tableData.push_back(21.8);
root_tableData.push_back(19.4);
root_tableData.push_back(16.7);
root_tableData.push_back(11.9);
root_tableData.push_back(6.2);
root_tableData.push_back(0.7);
root_tableData.push_back(-1.3);
root_tableData.push_back(-2.1);
root_tableData.push_back(-0.7);
root_tableData.push_back(3.2);
root_tableData.push_back(8.5);
root_tableData.push_back(13.1);
root_tableData.push_back(16.6);
root_tableData.push_back(18.6);
root_tableData.push_back(20.7);
root_tableData.push_back(22.7);
root_tableData.push_back(24.3);
root_tableData.push_back(26);
root_tableData.push_back(27.4);
root_tableData.push_back(28.4);
root_tableDims.clear();
root_tableDims.push_back(41);
EvalInterpTable* table_142 = new EvalInterpTable("table_142",EvalInterpTable::INTERP,EvalInterpTable::CLAMP,root_tableData);
table_142->SetBounds(0,560);
gp_root->AddEvalTable(table_142);
gp_node00059=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node00059->SetID("node00059");
gp_node00059->SetName("Choose Heat\nwave definition");
gp_node00059->SetInternal(false);
gp_node00059->SetExpr("2.0");
gp_node00059->SetTableID("");
gp_node00059->SetAsInt(true);
gp_node00059->SetAskVal(true);
gp_node00059->SetMinValue(1);
gp_node00059->SetMaxValue(2);
gp_node00059->SetExpectsFile(false);
gp_node00059->SetInitOnly(true);
gp_root->AddVariable(gp_node00059);

gp_node00062=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node00062->SetID("node00062");
gp_node00062->SetName("Numeric Heat\neffect mortality");
gp_node00062->SetInternal(false);
gp_node00062->SetExpr(" 0.023");
gp_node00062->SetTableID("");
gp_node00062->SetAskVal(true);
gp_node00062->SetMinValue(0.009999999776);
gp_node00062->SetMaxValue(0.03599999845);
gp_node00062->SetExpectsFile(false);
gp_node00062->SetInitOnly(true);
gp_root->AddVariable(gp_node00062);

gp_node00228=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node00228->SetID("node00228");
gp_node00228->SetName("Total area");
gp_node00228->SetInternal(false);
gp_node00228->SetExpr("sum([ AustinModel_determ.Decision_Units.Area ])");
gp_node00228->SetTableID("");
gp_node00228->SetExpectsFile(false);
gp_node00228->SetInitOnly(false);
gp_root->AddVariable(gp_node00228);

gp_node03602=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03602->SetID("node03602");
gp_node03602->SetName("total population");
gp_node03602->SetInternal(false);
gp_node03602->SetExpr("sum([AustinModel_determ.Decision_Units.population])");
gp_node03602->SetTableID("");
gp_node03602->SetExpectsFile(false);
gp_node03602->SetInitOnly(false);
gp_root->AddVariable(gp_node03602);

gp_node03603=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03603->SetID("node03603");
gp_node03603->SetName("Avr T");
gp_node03603->SetInternal(false);
gp_node03603->SetExpr("sum([ AustinModel_determ.Decision_Units.Average_air_temperatures ]*[ AustinModel_determ.Decision_Units.Area ])/sum([ AustinModel_determ.Decision_Units.Area ])");
gp_node03603->SetTableID("");
gp_node03603->SetExpectsFile(false);
gp_node03603->SetInitOnly(false);
gp_root->AddVariable(gp_node03603);

gp_node03604=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03604->SetID("node03604");
gp_node03604->SetName("Air T");
gp_node03604->SetInternal(false);
gp_node03604->SetExpr("(if time(  )==0.0 then 0.0 else (if AustinModel_determ.Choice_of_weather_data ==0.0 then element(element([[ AustinModel_determ.Yearly_Data.Weather.Temp_C ]], AustinModel_determ.JDAY ), AustinModel_determ.Year ) else  AustinModel_determ.GCC_D_Temp  end)+ 4.55 end)");
gp_node03604->SetTableID("");
gp_node03604->SetExpectsFile(false);
gp_node03604->SetInitOnly(false);
gp_root->AddVariable(gp_node03604);

gp_node03605=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03605->SetID("node03605");
gp_node03605->SetName("use NDVI data");
gp_node03605->SetInternal(false);
gp_node03605->SetExpr("0.0");
gp_node03605->SetTableID("");
gp_node03605->SetAsInt(true);
gp_node03605->SetAskVal(true);
gp_node03605->SetMinValue(0);
gp_node03605->SetMaxValue(1);
gp_node03605->SetExpectsFile(false);
gp_node03605->SetInitOnly(true);
gp_root->AddVariable(gp_node03605);

gp_node03607=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03607->SetID("node03607");
gp_node03607->SetName("Air_T reduction by water");
gp_node03607->SetInternal(false);
gp_node03607->SetExpr("2.0");
gp_node03607->SetTableID("");
gp_node03607->SetAsInt(true);
gp_node03607->SetAskVal(true);
gp_node03607->SetMinValue(0);
gp_node03607->SetMaxValue(5);
gp_node03607->SetExpectsFile(false);
gp_node03607->SetInitOnly(true);
gp_root->AddVariable(gp_node03607);

gp_node03612=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03612->SetID("node03612");
gp_node03612->SetName("Choice of\nweather data");
gp_node03612->SetInternal(false);
gp_node03612->SetExpr("(if AustinModel_determ.Year <27.0 then 0.0 else (if AustinModel_determ.Year <41.0 and  AustinModel_determ.Data_or_GCC_scenario ==0.0 then 0.0 else 1.0 end) end)");
gp_node03612->SetTableID("");
gp_node03612->SetExpectsFile(false);
gp_node03612->SetInitOnly(false);
gp_root->AddVariable(gp_node03612);

gp_node03614=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03614->SetID("node03614");
gp_node03614->SetName("Biweely period");
gp_node03614->SetInternal(false);
gp_node03614->SetExpr("firsttrue( AustinModel_determ.JDAY <=[8.0,21.0,35.0,49.0,63.0,77.0,91.0,105.0,119.0,133.0,147.0,161.0,175.0,189.0,203.0,217.0,231.0,245.0,259.0,273.0,287.0,301.0,315.0,329.0,343.0,357.0,372.0])");
gp_node03614->SetTableID("");
gp_node03614->SetExpectsFile(false);
gp_node03614->SetInitOnly(false);
gp_root->AddVariable(gp_node03614);

gp_node03617=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03617->SetID("node03617");
gp_node03617->SetName("Forest \nTemperatures");
gp_node03617->SetInternal(false);
gp_node03617->SetExpr("sum([ AustinModel_determ.Decision_Units.Forest_T ]*[ AustinModel_determ.Decision_Units.Area ])/sum([ AustinModel_determ.Decision_Units.Area ])");
gp_node03617->SetTableID("");
gp_node03617->SetExpectsFile(false);
gp_node03617->SetInitOnly(false);
gp_root->AddVariable(gp_node03617);

gp_node03619=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03619->SetID("node03619");
gp_node03619->SetName("Bare ground Temperatures");
gp_node03619->SetInternal(false);
gp_node03619->SetExpr("sum([ AustinModel_determ.Decision_Units.Bare_Ground_T ]*[ AustinModel_determ.Decision_Units.Area ])/sum([ AustinModel_determ.Decision_Units.Area ])");
gp_node03619->SetTableID("");
gp_node03619->SetExpectsFile(false);
gp_node03619->SetInitOnly(false);
gp_root->AddVariable(gp_node03619);

gp_node03620=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03620->SetID("node03620");
gp_node03620->SetName("R Humidity");
gp_node03620->SetInternal(false);
gp_node03620->SetExpr("(if time(  )==0.0 then 0.0 else (if AustinModel_determ.Choice_of_weather_data ==0.0 then element(element([[ AustinModel_determ.Yearly_Data.Weather.Humidity ]], AustinModel_determ.JDAY ), AustinModel_determ.Year ) else  AustinModel_determ.GCC_D_Relative_Humidity  end) end)");
gp_node03620->SetTableID("");
gp_node03620->SetExpectsFile(false);
gp_node03620->SetInitOnly(false);
gp_root->AddVariable(gp_node03620);

gp_node03626=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03626->SetID("node03626");
gp_node03626->SetName("Start year");
gp_node03626->SetInternal(false);
gp_node03626->SetExpr("2011.0");
gp_node03626->SetTableID("");
gp_node03626->SetAsInt(true);
gp_node03626->SetAskVal(true);
gp_node03626->SetMinValue(1973);
gp_node03626->SetMaxValue(2099);
gp_node03626->SetExpectsFile(false);
gp_node03626->SetInitOnly(true);
gp_root->AddVariable(gp_node03626);

gp_node03628=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03628->SetID("node03628");
gp_node03628->SetName("VPD");
gp_node03628->SetInternal(false);
gp_node03628->SetExpr(" AustinModel_determ.Saturated_pressure *(1.0- AustinModel_determ.R_Humidity /100.0)");
gp_node03628->SetTableID("");
gp_node03628->SetExpectsFile(false);
gp_node03628->SetInitOnly(false);
gp_root->AddVariable(gp_node03628);

gp_node03630=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03630->SetID("node03630");
gp_node03630->SetName("temp reduction\nper unit of NDVI");
gp_node03630->SetInternal(false);
gp_node03630->SetExpr(" 39.69 * AustinModel_determ.VPD /( 12.21+ AustinModel_determ.VPD )");
gp_node03630->SetTableID("");
gp_node03630->SetExpectsFile(false);
gp_node03630->SetInitOnly(false);
gp_root->AddVariable(gp_node03630);

gp_node03632=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03632->SetID("node03632");
gp_node03632->SetName("Bare Soil\nTemperature");
gp_node03632->SetInternal(false);
gp_node03632->SetExpr("( AustinModel_determ.Air_T - 3.1876)/ 0.6443");
gp_node03632->SetTableID("");
gp_node03632->SetAskVal(true);
gp_node03632->SetMinValue(-50);
gp_node03632->SetMaxValue(50);
gp_node03632->SetExpectsFile(true);
gp_node03632->SetInitOnly(false);
gp_root->AddVariable(gp_node03632);

gp_node03634=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03634->SetID("node03634");
gp_node03634->SetName("Saturated\npressure");
gp_node03634->SetInternal(false);
gp_node03634->SetExpr(" 0.61078 *exp( 17.269* AustinModel_determ.Bare_Soil_Temperature /( 237.3+ AustinModel_determ.Bare_Soil_Temperature ))");
gp_node03634->SetTableID("");
gp_node03634->SetExpectsFile(false);
gp_node03634->SetInitOnly(false);
gp_root->AddVariable(gp_node03634);

gp_node03638=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03638->SetID("node03638");
gp_node03638->SetName("JDAY");
gp_node03638->SetInternal(false);
gp_node03638->SetExpr("fmod(time(  ),365.0)+1.0");
gp_node03638->SetTableID("");
gp_node03638->SetExpectsFile(false);
gp_node03638->SetInitOnly(false);
gp_root->AddVariable(gp_node03638);

gp_node03640=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03640->SetID("node03640");
gp_node03640->SetName("Regional\nThreshold T");
gp_node03640->SetInternal(false);
gp_node03640->SetExpr(" 28.28");
gp_node03640->SetTableID("");
gp_node03640->SetAskVal(true);
gp_node03640->SetMinValue(25);
gp_node03640->SetMaxValue(35);
gp_node03640->SetExpectsFile(false);
gp_node03640->SetInitOnly(true);
gp_root->AddVariable(gp_node03640);

gp_node03651=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03651->SetID("node03651");
gp_node03651->SetName("GCC D\nRelative\nHumidity");
gp_node03651->SetInternal(false);
gp_node03651->SetExpr(" 0.5");
gp_node03651->SetTableID("");
gp_node03651->SetExpectsFile(false);
gp_node03651->SetInitOnly(true);
gp_root->AddVariable(gp_node03651);

gp_node03653=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03653->SetID("node03653");
gp_node03653->SetName("GCC D Temp");
gp_node03653->SetInternal(false);
gp_node03653->SetExpr("82.0");
gp_node03653->SetTableID("");
gp_node03653->SetExpectsFile(false);
gp_node03653->SetInitOnly(true);
gp_root->AddVariable(gp_node03653);

gp_node03668=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03668->SetID("node03668");
gp_node03668->SetName("Windspeed");
gp_node03668->SetInternal(false);
gp_node03668->SetExpr("(if time(  )==0.0 then 0.0 else (if AustinModel_determ.Choice_of_weather_data ==0.0 then element(element([[ AustinModel_determ.Yearly_Data.Weather.Windspeed ]], AustinModel_determ.JDAY ), AustinModel_determ.Year ) else sum([ AustinModel_determ.GCC_input.GCC_Wind_speeds ]) end) end)");
gp_node03668->SetTableID("");
gp_node03668->SetExpectsFile(false);
gp_node03668->SetInitOnly(false);
gp_root->AddVariable(gp_node03668);

gp_node03669=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03669->SetID("node03669");
gp_node03669->SetName("month");
gp_node03669->SetInternal(false);
gp_node03669->SetExpr("firsttrue( AustinModel_determ.JDAY <=[31.0,59.0,90.0,120.0,151.0,181.0,212.0,243.0,273.0,304.0,334.0,365.0])");
gp_node03669->SetTableID("");
gp_node03669->SetExpectsFile(false);
gp_node03669->SetInitOnly(false);
gp_root->AddVariable(gp_node03669);

gp_node03671=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03671->SetID("node03671");
gp_node03671->SetName("Data or GCC scenario");
gp_node03671->SetInternal(false);
gp_node03671->SetExpr("0.0");
gp_node03671->SetTableID("");
gp_node03671->SetAsInt(true);
gp_node03671->SetAskVal(true);
gp_node03671->SetMinValue(0);
gp_node03671->SetMaxValue(1);
gp_node03671->SetExpectsFile(false);
gp_node03671->SetInitOnly(true);
gp_root->AddVariable(gp_node03671);

gp_node03672=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03672->SetID("node03672");
gp_node03672->SetName("temp limit sizer");
gp_node03672->SetInternal(false);
gp_node03672->SetExpr("1.0");
gp_node03672->SetTableID("");
gp_node03672->SetAsInt(true);
gp_node03672->SetAskVal(true);
gp_node03672->SetMinValue(-2);
gp_node03672->SetMaxValue(2);
gp_node03672->SetExpectsFile(false);
gp_node03672->SetInitOnly(true);
gp_root->AddVariable(gp_node03672);

gp_node03680=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03680->SetID("node03680");
gp_node03680->SetName("Init RH");
gp_node03680->SetInternal(false);
gp_node03680->SetExpr("AustinModel_determ.averaging_time* 66.2");
gp_node03680->SetTableID("");
gp_node03680->SetExpectsFile(false);
gp_node03680->SetInitOnly(false);
gp_root->AddVariable(gp_node03680);

gp_node03682=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03682->SetID("node03682");
gp_node03682->SetName("averaging time");
gp_node03682->SetInternal(false);
gp_node03682->SetExpr("14.0");
gp_node03682->SetTableID("");
gp_node03682->SetAsInt(true);
gp_node03682->SetAskVal(true);
gp_node03682->SetMinValue(1);
gp_node03682->SetMaxValue(30);
gp_node03682->SetExpectsFile(false);
gp_node03682->SetInitOnly(true);
gp_root->AddVariable(gp_node03682);

gp_node03683=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03683->SetID("node03683");
gp_node03683->SetName("RA_Temp");
gp_node03683->SetInternal(false);
gp_node03683->SetExpr(" AustinModel_determ.T /AustinModel_determ.averaging_time");
gp_node03683->SetTableID("");
gp_node03683->SetExpectsFile(false);
gp_node03683->SetInitOnly(false);
gp_root->AddVariable(gp_node03683);

gp_node03685=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03685->SetID("node03685");
gp_node03685->SetName("RA_Rel Hum");
gp_node03685->SetInternal(false);
gp_node03685->SetExpr(" AustinModel_determ.RH /(AustinModel_determ.averaging_time+1.0)");
gp_node03685->SetTableID("");
gp_node03685->SetExpectsFile(false);
gp_node03685->SetInitOnly(false);
gp_root->AddVariable(gp_node03685);

gp_node03693=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03693->SetID("node03693");
gp_node03693->SetName("GCC Rainfall");
gp_node03693->SetInternal(false);
gp_node03693->SetExpr("sum([ AustinModel_determ.GCC_input.Precip_sc ])*element(element([[ AustinModel_determ.Ratio_of_rain_by_month ]], AustinModel_determ.Chance_Position ),AustinModel_determ.month)*3.0");
gp_node03693->SetTableID("");
gp_node03693->SetExpectsFile(false);
gp_node03693->SetInitOnly(false);
gp_root->AddVariable(gp_node03693);

gp_node03695=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03695->SetID("node03695");
gp_node03695->SetName("Choice of Social Fabric\nIndicator");
gp_node03695->SetInternal(false);
gp_node03695->SetExpr("2.0");
gp_node03695->SetTableID("");
gp_node03695->SetAsInt(true);
gp_node03695->SetAskVal(true);
gp_node03695->SetMinValue(1);
gp_node03695->SetMaxValue(5);
gp_node03695->SetExpectsFile(false);
gp_node03695->SetInitOnly(true);
gp_root->AddVariable(gp_node03695);

gp_node03697=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03697->SetID("node03697");
gp_node03697->SetName("NDVI year");
gp_node03697->SetInternal(false);
gp_node03697->SetExpr(" AustinModel_determ.Year -31.0");
gp_node03697->SetTableID("");
gp_node03697->SetExpectsFile(false);
gp_node03697->SetInitOnly(false);
gp_root->AddVariable(gp_node03697);

gp_node03702=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03702->SetID("node03702");
gp_node03702->SetName("Rain Intensity");
gp_node03702->SetInternal(false);
gp_node03702->SetExpr("10.0");
gp_node03702->SetTableID("");
gp_node03702->SetExpectsFile(false);
gp_node03702->SetInitOnly(true);
gp_root->AddVariable(gp_node03702);

gp_node03704=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03704->SetID("node03704");
gp_node03704->SetName("Chance\nPosition");
gp_node03704->SetInternal(false);
gp_node03704->SetExpr("firsttrue( AustinModel_determ.Rain_Intensity >=[ 99.5, 97.5,90.0,75.0,50.0,25.0,10.0, 2.5, 0.5, 0.1,0.0])");
gp_node03704->SetTableID("");
gp_node03704->SetExpectsFile(false);
gp_node03704->SetInitOnly(false);
gp_root->AddVariable(gp_node03704);

gp_node03707=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03707->SetID("node03707");
gp_node03707->SetName("Rain");
gp_node03707->SetInternal(false);
gp_node03707->SetExpr("(if time(  )==0.0 then 0.0 else (if AustinModel_determ.Choice_of_weather_data ==0.0 then element(element([[ AustinModel_determ.Yearly_Data.Weather.PREC ]], AustinModel_determ.JDAY ), AustinModel_determ.Year ) else  AustinModel_determ.GCC_Rainfall  end) end)");
gp_node03707->SetTableID("");
gp_node03707->SetExpectsFile(false);
gp_node03707->SetInitOnly(false);
gp_root->AddVariable(gp_node03707);

gp_node03709=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03709->SetID("node03709");
gp_node03709->SetName("Averaged RA_Temp");
gp_node03709->SetInternal(false);
gp_node03709->SetExpr("graph( AustinModel_determ.JDAY )");
gp_node03709->SetTableID("table_142");
gp_node03709->SetAskVal(true);
gp_node03709->SetMinValue(-10);
gp_node03709->SetMaxValue(50);
gp_node03709->SetExpectsFile(false);
gp_node03709->SetInitOnly(false);
gp_root->AddVariable(gp_node03709);

gp_node03712=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03712->SetID("node03712");
gp_node03712->SetName("NDVI data\nor predictions");
gp_node03712->SetInternal(false);
gp_node03712->SetExpr("(if AustinModel_determ.Year <32.0 or  AustinModel_determ.Year >=40.0 or  AustinModel_determ.Year ==39.0 and  AustinModel_determ.Biweely_period >19.0 then 1.0 else (if AustinModel_determ.use_NDVI_data==0.0 then 0.0 else 1.0 end) end)");
gp_node03712->SetTableID("");
gp_node03712->SetAsInt(true);
gp_node03712->SetAskVal(true);
gp_node03712->SetMinValue(0);
gp_node03712->SetMaxValue(1);
gp_node03712->SetExpectsFile(false);
gp_node03712->SetInitOnly(false);
gp_root->AddVariable(gp_node03712);

gp_node03718=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03718->SetID("node03718");
gp_node03718->SetName("Heat Index in C");
gp_node03718->SetInternal(false);
gp_node03718->SetExpr(" AustinModel_determ.HeatIndex *5.0/9.0-32.0");
gp_node03718->SetTableID("");
gp_node03718->SetExpectsFile(false);
gp_node03718->SetInitOnly(false);
gp_root->AddVariable(gp_node03718);

gp_node03720=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03720->SetID("node03720");
gp_node03720->SetName("EMS modeled");
gp_node03720->SetInternal(false);
gp_node03720->SetExpr("element(sum([[ AustinModel_determ.Displays.M_estimates.Morbidity ]]),1.0)");
gp_node03720->SetTableID("");
gp_node03720->SetExpectsFile(false);
gp_node03720->SetInitOnly(false);
gp_root->AddVariable(gp_node03720);

gp_node03723=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03723->SetID("node03723");
gp_node03723->SetName("EMS cat");
gp_node03723->SetInternal(false);
gp_node03723->SetExpr("element(sum([[ AustinModel_determ.Displays.M_estimates.Morbidity ]]),3.0)");
gp_node03723->SetTableID("");
gp_node03723->SetExpectsFile(false);
gp_node03723->SetInitOnly(false);
gp_root->AddVariable(gp_node03723);

gp_node03725=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03725->SetID("node03725");
gp_node03725->SetName("EMS num");
gp_node03725->SetInternal(false);
gp_node03725->SetExpr("element(sum([[ AustinModel_determ.Displays.M_estimates.Morbidity ]]),2.0)");
gp_node03725->SetTableID("");
gp_node03725->SetExpectsFile(false);
gp_node03725->SetInitOnly(false);
gp_root->AddVariable(gp_node03725);

gp_node03594=(VarArray*)TestModelBm->GenModelObject(OME_VARARRAY);
gp_node03594->SetID("node03594");
gp_node03594->SetName("Total Mortalities");
gp_node03594->SetInternal(false);
gp_node03594->SetExpr("sum([[AustinModel_determ.Displays.M_estimates.cumulative_morbidity]])");
gp_node03594->SetTableID("");
gp_node03594->SetExpectsFile(false);
gp_node03594->SetInitOnly(false);
IndVec node03594_dims=IndVec(1);
node03594_dims[0]=5;
gp_node03594->OverrideDims(node03594_dims);
gp_root->AddVariable(gp_node03594);

gp_node03596=(VarArray*)TestModelBm->GenModelObject(OME_VARARRAY);
gp_node03596->SetID("node03596");
gp_node03596->SetName("Children");
gp_node03596->SetInternal(false);
gp_node03596->SetExpr("[33.0,17.0,1.0,34.0,18.0,2.0,35.0,19.0,3.0,36.0,20.0,4.0,37.0,21.0,5.0,38.0,22.0,6.0,39.0,23.0,7.0,40.0,24.0,8.0,41.0,25.0,9.0,42.0,26.0,10.0,43.0,27.0,11.0,44.0,28.0,12.0,45.0,29.0,13.0,46.0,30.0,14.0,47.0,31.0,15.0,48.0,32.0,16.0]");
gp_node03596->SetTableID("");
gp_node03596->SetExpectsFile(false);
gp_node03596->SetInitOnly(false);
IndVec node03596_dims=IndVec(1);
node03596_dims[0]=48;
gp_node03596->OverrideDims(node03596_dims);
gp_root->AddVariable(gp_node03596);

gp_node03598=(VarArray*)TestModelBm->GenModelObject(OME_VARARRAY);
gp_node03598->SetID("node03598");
gp_node03598->SetName("None White");
gp_node03598->SetInternal(false);
gp_node03598->SetExpr("[33.0,49.0,65.0,81.0,97.0,17.0,113.0,129.0,145.0,161.0,177.0,193.0,209.0,1.0,34.0,50.0,66.0,82.0,98.0,18.0,114.0,130.0,146.0,162.0,178.0,194.0,210.0,2.0,35.0,51.0,67.0,83.0,99.0,19.0,115.0,131.0,147.0,163.0,179.0,195.0,211.0,3.0,36.0,52.0,68.0,84.0,100.0,20.0,116.0,132.0,148.0,164.0,180.0,196.0,212.0,4.0,37.0,53.0,69.0,85.0,101.0,21.0,117.0,133.0,149.0,165.0,181.0,197.0,213.0,5.0,38.0,54.0,70.0,86.0,102.0,22.0,118.0,134.0,150.0,166.0,182.0,198.0,214.0,6.0,41.0,57.0,73.0,89.0,105.0,25.0,121.0,137.0,153.0,169.0,185.0,201.0,217.0,9.0,42.0,58.0,74.0,90.0,106.0,26.0,122.0,138.0,154.0,170.0,186.0,202.0,218.0,10.0,43.0,59.0,75.0,91.0,107.0,27.0,123.0,139.0,155.0,171.0,187.0,203.0,219.0,11.0,44.0,60.0,76.0,92.0,108.0,28.0,124.0,140.0,156.0,172.0,188.0,204.0,220.0,12.0,45.0,61.0,77.0,93.0,109.0,29.0,125.0,141.0,157.0,173.0,189.0,205.0,221.0,13.0,46.0,62.0,78.0,94.0,110.0,30.0,126.0,142.0,158.0,174.0,190.0,206.0,222.0,14.0]");
gp_node03598->SetTableID("");
gp_node03598->SetExpectsFile(false);
gp_node03598->SetInitOnly(false);
IndVec node03598_dims=IndVec(1);
node03598_dims[0]=168;
gp_node03598->OverrideDims(node03598_dims);
gp_root->AddVariable(gp_node03598);

gp_node03600=(VarArray*)TestModelBm->GenModelObject(OME_VARARRAY);
gp_node03600->SetID("node03600");
gp_node03600->SetName("Elderly");
gp_node03600->SetInternal(false);
gp_node03600->SetExpr("[145.0,161.0,177.0,193.0,209.0,146.0,162.0,178.0,194.0,210.0,147.0,163.0,179.0,195.0,211.0,148.0,164.0,180.0,196.0,212.0,149.0,165.0,181.0,197.0,213.0,150.0,166.0,182.0,198.0,214.0,151.0,167.0,183.0,199.0,215.0,152.0,168.0,184.0,200.0,216.0,153.0,169.0,185.0,201.0,217.0,154.0,170.0,186.0,202.0,218.0,155.0,171.0,187.0,203.0,219.0,156.0,172.0,188.0,204.0,220.0,157.0,173.0,189.0,205.0,221.0,158.0,174.0,190.0,206.0,222.0,159.0,175.0,191.0,207.0,223.0,160.0,176.0,192.0,208.0,224.0]");
gp_node03600->SetTableID("");
gp_node03600->SetExpectsFile(false);
gp_node03600->SetInitOnly(false);
IndVec node03600_dims=IndVec(1);
node03600_dims[0]=80;
gp_node03600->OverrideDims(node03600_dims);
gp_root->AddVariable(gp_node03600);

gp_node03641=(VarArray*)TestModelBm->GenModelObject(OME_VARARRAY);
gp_node03641->SetID("node03641");
gp_node03641->SetName("Blgr_ids");
gp_node03641->SetInternal(false);
gp_node03641->SetExpr("makearray(1.0,696.0)");
gp_node03641->SetTableID("");
gp_node03641->SetAsInt(true);
gp_node03641->SetExpectsFile(false);
gp_node03641->SetInitOnly(false);
IndVec node03641_dims=IndVec(0);
gp_node03641->OverrideDims(node03641_dims);
gp_root->AddVariable(gp_node03641);

gp_node03656=(VarArray*)TestModelBm->GenModelObject(OME_VARARRAY);
gp_node03656->SetID("node03656");
gp_node03656->SetName("RH St Dev");
gp_node03656->SetInternal(false);
gp_node03656->SetExpr("makearray(1.0,12.0)");
gp_node03656->SetTableID("");
gp_node03656->SetAsInt(true);
gp_node03656->SetExpectsFile(false);
gp_node03656->SetInitOnly(false);
IndVec node03656_dims=IndVec(0);
gp_node03656->OverrideDims(node03656_dims);
gp_root->AddVariable(gp_node03656);

gp_node03657=(VarArray*)TestModelBm->GenModelObject(OME_VARARRAY);
gp_node03657->SetID("node03657");
gp_node03657->SetName("Temp C St Dev");
gp_node03657->SetInternal(false);
gp_node03657->SetExpr("makearray(1.0,12.0)");
gp_node03657->SetTableID("");
gp_node03657->SetAsInt(true);
gp_node03657->SetExpectsFile(false);
gp_node03657->SetInitOnly(false);
IndVec node03657_dims=IndVec(0);
gp_node03657->OverrideDims(node03657_dims);
gp_root->AddVariable(gp_node03657);

gp_node03674=(VarArray*)TestModelBm->GenModelObject(OME_VARARRAY);
gp_node03674->SetID("node03674");
gp_node03674->SetName("Wealth Upper boundaries");
gp_node03674->SetInternal(false);
gp_node03674->SetExpr("[10000.0,15000.0,20000.0,25000.0,30000.0,35000.0,40000.0,45000.0,50000.0,60000.0,70000.0,100000.0,125000.0,150000.0,200000.0,20000000.0]");
gp_node03674->SetTableID("");
gp_node03674->SetExpectsFile(false);
gp_node03674->SetInitOnly(false);
IndVec node03674_dims=IndVec(1);
node03674_dims[0]=16;
gp_node03674->OverrideDims(node03674_dims);
gp_root->AddVariable(gp_node03674);

gp_node03677=(VarArray*)TestModelBm->GenModelObject(OME_VARARRAY);
gp_node03677->SetID("node03677");
gp_node03677->SetName("LandUse air T");
gp_node03677->SetInternal(false);
gp_node03677->SetExpr("sum([[ AustinModel_determ.Decision_Units.Land_Covers.Air_temp ]])/696.0");
gp_node03677->SetTableID("");
gp_node03677->SetExpectsFile(false);
gp_node03677->SetInitOnly(false);
IndVec node03677_dims=IndVec(1);
node03677_dims[0]=15;
gp_node03677->OverrideDims(node03677_dims);
gp_root->AddVariable(gp_node03677);

gp_node03696=(VarArray*)TestModelBm->GenModelObject(OME_VARARRAY);
gp_node03696->SetID("node03696");
gp_node03696->SetName("LandUse Canopy T");
gp_node03696->SetInternal(false);
gp_node03696->SetExpr("sum([[ AustinModel_determ.Decision_Units.Land_Covers.Canopy_temp ]])/696.0");
gp_node03696->SetTableID("");
gp_node03696->SetExpectsFile(false);
gp_node03696->SetInitOnly(false);
IndVec node03696_dims=IndVec(1);
node03696_dims[0]=15;
gp_node03696->OverrideDims(node03696_dims);
gp_root->AddVariable(gp_node03696);

gp_node03699=(VarArray*)TestModelBm->GenModelObject(OME_VARARRAY);
gp_node03699->SetID("node03699");
gp_node03699->SetName("NDVIs");
gp_node03699->SetInternal(false);
gp_node03699->SetExpr("(if AustinModel_determ.NDVI_data_or_predictions ==1.0 then 0.0 else element(element([[[ AustinModel_determ.NDVI_years.biweekly_period.NDVI_data ]]], AustinModel_determ.NDVI_year ), AustinModel_determ.Biweely_period ) end)");
gp_node03699->SetTableID("");
gp_node03699->SetExpectsFile(false);
gp_node03699->SetInitOnly(false);
IndVec node03699_dims=IndVec(1);
node03699_dims[0]=9;
gp_node03699->OverrideDims(node03699_dims);
gp_root->AddVariable(gp_node03699);

gp_node03706=(VarArray*)TestModelBm->GenModelObject(OME_VARARRAY);
gp_node03706->SetID("node03706");
gp_node03706->SetName("Ratio\nof rain\nby month");
gp_node03706->SetInternal(false);
gp_node03706->SetExpr("makearray(makearray(1.0,12.0),11.0)");
gp_node03706->SetTableID("");
gp_node03706->SetExpectsFile(false);
gp_node03706->SetInitOnly(false);
IndVec node03706_dims=IndVec(0);
gp_node03706->OverrideDims(node03706_dims);
gp_root->AddVariable(gp_node03706);

gp_node03728=(VarArray*)TestModelBm->GenModelObject(OME_VARARRAY);
gp_node03728->SetID("node03728");
gp_node03728->SetName("Temp red");
gp_node03728->SetInternal(false);
gp_node03728->SetExpr("sum([[AustinModel_determ.Decision_Units.Land_Covers.temperature_reduction]])/696.0");
gp_node03728->SetTableID("");
gp_node03728->SetExpectsFile(false);
gp_node03728->SetInitOnly(false);
IndVec node03728_dims=IndVec(1);
node03728_dims[0]=15;
gp_node03728->OverrideDims(node03728_dims);
gp_root->AddVariable(gp_node03728);

gp_node00060=(TimeSeriesVar*)TestModelBm->GenModelObject(OME_TSVAR);
gp_node00060->SetID("node00060");
gp_node00060->SetName("Mortality Choice");
gp_node00060->SetInternal(false);
gp_node00060->SetExpr("");
gp_node00060->SetTableID("");
gp_node00060->SetAsInt(true);
gp_node00060->SetAskVal(true);
gp_node00060->SetMinValue(1);
gp_node00060->SetMaxValue(3);
gp_node00060->SetExpectsFile(false);
gp_node00060->SetInitOnly(false);
gp_node00060->SetInterval(0);
gp_node00060->SetInterpolation(TimeSeriesVar::LAST);
gp_root->AddVariable(gp_node00060);

gp_node03637=(TimeSeriesVar*)TestModelBm->GenModelObject(OME_TSVAR);
gp_node03637->SetID("node03637");
gp_node03637->SetName("Choose LandCover");
gp_node03637->SetInternal(false);
gp_node03637->SetExpr("");
gp_node03637->SetTableID("");
gp_node03637->SetAsInt(true);
gp_node03637->SetAskVal(true);
gp_node03637->SetMinValue(1);
gp_node03637->SetMaxValue(15);
gp_node03637->SetExpectsFile(false);
gp_node03637->SetInitOnly(false);
gp_node03637->SetInterval(0);
gp_node03637->SetInterpolation(TimeSeriesVar::LAST);
gp_root->AddVariable(gp_node03637);

gp_node03648=(TimeSeriesVar*)TestModelBm->GenModelObject(OME_TSVAR);
gp_node03648->SetID("node03648");
gp_node03648->SetName("HeatIndex");
gp_node03648->SetInternal(false);
gp_node03648->SetExpr("");
gp_node03648->SetTableID("");
gp_node03648->SetAsInt(true);
gp_node03648->SetAskVal(true);
gp_node03648->SetMinValue(0);
gp_node03648->SetMaxValue(110);
gp_node03648->SetExpectsFile(false);
gp_node03648->SetInitOnly(false);
gp_node03648->SetInterval(0);
gp_node03648->SetInterpolation(TimeSeriesVar::LAST);
gp_root->AddVariable(gp_node03648);

gp_node03694=(TimeSeriesVar*)TestModelBm->GenModelObject(OME_TSVAR);
gp_node03694->SetID("node03694");
gp_node03694->SetName("Choose Heat Mortality Index");
gp_node03694->SetInternal(false);
gp_node03694->SetExpr("");
gp_node03694->SetTableID("");
gp_node03694->SetAsInt(true);
gp_node03694->SetAskVal(true);
gp_node03694->SetMinValue(1);
gp_node03694->SetMaxValue(5);
gp_node03694->SetExpectsFile(false);
gp_node03694->SetInitOnly(false);
gp_node03694->SetInterval(0);
gp_node03694->SetInterpolation(TimeSeriesVar::LAST);
gp_root->AddVariable(gp_node03694);

gp_node03722=(TimeSeriesVar*)TestModelBm->GenModelObject(OME_TSVAR);
gp_node03722->SetID("node03722");
gp_node03722->SetName("EMS measured");
gp_node03722->SetInternal(false);
gp_node03722->SetExpr("");
gp_node03722->SetTableID("");
gp_node03722->SetAsInt(true);
gp_node03722->SetAskVal(true);
gp_node03722->SetMinValue(0);
gp_node03722->SetMaxValue(10);
gp_node03722->SetExpectsFile(false);
gp_node03722->SetInitOnly(false);
gp_node03722->SetInterval(0);
gp_node03722->SetInterpolation(TimeSeriesVar::LAST);
gp_root->AddVariable(gp_node03722);

gp_node03622=(StateVar*)TestModelBm->GenModelObject(OME_STATEVAR);
gp_node03622->SetID("node03622");
gp_node03622->SetName("Year");
gp_node03622->SetInternal(false);
gp_node03622->SetExpr(" AustinModel_determ.Start_year -1973.0");
gp_node03622->SetTableID("");
gp_root->AddStateVar(gp_node03622);

gp_node03646=(StateVar*)TestModelBm->GenModelObject(OME_STATEVAR);
gp_node03646->SetID("node03646");
gp_node03646->SetName("T");
gp_node03646->SetInternal(false);
gp_node03646->SetExpr("AustinModel_determ.averaging_time* 9.7");
gp_node03646->SetTableID("");
gp_root->AddStateVar(gp_node03646);

gp_node03663=(StateVar*)TestModelBm->GenModelObject(OME_STATEVAR);
gp_node03663->SetID("node03663");
gp_node03663->SetName("RH");
gp_node03663->SetInternal(false);
gp_node03663->SetExpr(" AustinModel_determ.Init_RH ");
gp_node03663->SetTableID("");
gp_root->AddStateVar(gp_node03663);

gp_node03687=(StateVar*)TestModelBm->GenModelObject(OME_STATEVAR);
gp_node03687->SetID("node03687");
gp_node03687->SetName("P");
gp_node03687->SetInternal(false);
gp_node03687->SetExpr("0.0");
gp_node03687->SetTableID("");
gp_node03687->SetInitCondition(0);
gp_root->AddStateVar(gp_node03687);

gp_arc03874=(Flow*)TestModelBm->GenModelObject(OME_FLOW);
gp_arc03874->SetID("arc03874");
gp_arc03874->SetName("plus year");
gp_arc03874->SetInternal(false);
gp_arc03874->SetExpr("(if time(  )==0.0 or  AustinModel_determ.JDAY ==365.0 then 1.0 else 0.0 end)");
gp_arc03874->SetTableID("");
gp_arc03874->SetTargetID("node03622");
gp_root->AddFlow(gp_arc03874);

gp_arc04241=(Flow*)TestModelBm->GenModelObject(OME_FLOW);
gp_arc04241->SetID("arc04241");
gp_arc04241->SetName("add T");
gp_arc04241->SetInternal(false);
gp_arc04241->SetExpr(" AustinModel_determ.Air_T ");
gp_arc04241->SetTableID("");
gp_arc04241->SetTargetID("node03646");
gp_root->AddFlow(gp_arc04241);

gp_arc04229=(Flow*)TestModelBm->GenModelObject(OME_FLOW);
gp_arc04229->SetID("arc04229");
gp_arc04229->SetName("remove T");
gp_arc04229->SetInternal(false);
gp_arc04229->SetExpr("var_delay(AustinModel_determ.add_T,if time(  )<AustinModel_determ.averaging_time then 1.0 else AustinModel_determ.averaging_time end)");
gp_arc04229->SetTableID("");
gp_arc04229->SetSourceID("node03646");
gp_root->AddFlow(gp_arc04229);

gp_arc04232=(Flow*)TestModelBm->GenModelObject(OME_FLOW);
gp_arc04232->SetID("arc04232");
gp_arc04232->SetName("remove RH");
gp_arc04232->SetInternal(false);
gp_arc04232->SetExpr("var_delay( AustinModel_determ.Add_RH ,if time(  )<AustinModel_determ.averaging_time then 1.0 else AustinModel_determ.averaging_time end)");
gp_arc04232->SetTableID("");
gp_arc04232->SetSourceID("node03663");
gp_root->AddFlow(gp_arc04232);

gp_arc04234=(Flow*)TestModelBm->GenModelObject(OME_FLOW);
gp_arc04234->SetID("arc04234");
gp_arc04234->SetName("Add RH");
gp_arc04234->SetInternal(false);
gp_arc04234->SetExpr(" AustinModel_determ.R_Humidity ");
gp_arc04234->SetTableID("");
gp_arc04234->SetTargetID("node03663");
gp_root->AddFlow(gp_arc04234);

gp_arc04266=(Flow*)TestModelBm->GenModelObject(OME_FLOW);
gp_arc04266->SetID("arc04266");
gp_arc04266->SetName("add precip");
gp_arc04266->SetInternal(false);
gp_arc04266->SetExpr(" AustinModel_determ.Rain ");
gp_arc04266->SetTableID("");
gp_arc04266->SetTargetID("node03687");
gp_root->AddFlow(gp_arc04266);

gp_arc04268=(Flow*)TestModelBm->GenModelObject(OME_FLOW);
gp_arc04268->SetID("arc04268");
gp_arc04268->SetName("remove precip");
gp_arc04268->SetInternal(false);
gp_arc04268->SetExpr("var_delay(AustinModel_determ.add_precip,if time(  )<AustinModel_determ.averaging_time then 1.0 else AustinModel_determ.averaging_time end)");
gp_arc04268->SetTableID("");
gp_arc04268->SetSourceID("node03687");
gp_root->AddFlow(gp_arc04268);

gp_node03593=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03593->SetID("node03593");
gp_node03593->SetName("39 years of Lakesurface Evap");
gp_node03593->SetInternal(false);
gp_node03593->SetInitNumInstances(39);
gp_node03593->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node03732=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03732->SetID("node03732");
gp_node03732->SetName("Per month");
gp_node03732->SetInternal(false);
gp_node03732->SetInitNumInstances(12);
gp_node03732->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node03734=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03734->SetID("node03734");
gp_node03734->SetName("Texas Water\nDevelopment Board");
gp_node03734->SetInternal(false);
gp_node03734->SetExpr("");
gp_node03734->SetTableID("");
gp_node03734->SetExpectsFile(true);
gp_node03734->SetInitOnly(true);
gp_node03732->AddVariable(gp_node03734);

gp_node03593->AddSubmodel(gp_node03732);

gp_root->AddSubmodel(gp_node03593);

gp_node03595=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03595->SetID("node03595");
gp_node03595->SetName("Hazard Risk Factors");
gp_node03595->SetInternal(false);
gp_node03595->SetInitNumInstances(10);
gp_node03595->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node03739=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03739->SetID("node03739");
gp_node03739->SetName("Race Eth Gen Age");
gp_node03739->SetInternal(false);
gp_node03739->SetInitNumInstances(224);
gp_node03739->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node03744=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03744->SetID("node03744");
gp_node03744->SetName("E Health Outcomes");
gp_node03744->SetInternal(false);
gp_node03744->SetInitNumInstances(11);
gp_node03744->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node03746=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03746->SetID("node03746");
gp_node03746->SetName("Race Eth Gen Age\nVulnerability");
gp_node03746->SetInternal(false);
gp_node03746->SetExpr("");
gp_node03746->SetTableID("");
gp_node03746->SetExpectsFile(true);
gp_node03746->SetInitOnly(true);
gp_node03744->AddVariable(gp_node03746);

gp_node03739->AddSubmodel(gp_node03744);

gp_node03595->AddSubmodel(gp_node03739);

gp_node03740=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03740->SetID("node03740");
gp_node03740->SetName("Employment");
gp_node03740->SetInternal(false);
gp_node03740->SetInitNumInstances(2);
gp_node03740->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node03749=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03749->SetID("node03749");
gp_node03749->SetName("E Health Outcomes");
gp_node03749->SetInternal(false);
gp_node03749->SetInitNumInstances(11);
gp_node03749->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node03751=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03751->SetID("node03751");
gp_node03751->SetName("Employment\nVulnerability");
gp_node03751->SetInternal(false);
gp_node03751->SetExpr("");
gp_node03751->SetTableID("");
gp_node03751->SetExpectsFile(true);
gp_node03751->SetInitOnly(true);
gp_node03749->AddVariable(gp_node03751);

gp_node03740->AddSubmodel(gp_node03749);

gp_node03595->AddSubmodel(gp_node03740);

gp_node03741=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03741->SetID("node03741");
gp_node03741->SetName("Life Style");
gp_node03741->SetInternal(false);
gp_node03741->SetInitNumInstances(2);
gp_node03741->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node03753=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03753->SetID("node03753");
gp_node03753->SetName("L Health Outcomes");
gp_node03753->SetInternal(false);
gp_node03753->SetInitNumInstances(11);
gp_node03753->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node03755=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03755->SetID("node03755");
gp_node03755->SetName("Life\nStyle\nVulnerability");
gp_node03755->SetInternal(false);
gp_node03755->SetExpr("");
gp_node03755->SetTableID("");
gp_node03755->SetExpectsFile(true);
gp_node03755->SetInitOnly(true);
gp_node03753->AddVariable(gp_node03755);

gp_node03741->AddSubmodel(gp_node03753);

gp_node03595->AddSubmodel(gp_node03741);

gp_node03743=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03743->SetID("node03743");
gp_node03743->SetName("Income groups");
gp_node03743->SetInternal(false);
gp_node03743->SetInitNumInstances(16);
gp_node03743->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node03756=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03756->SetID("node03756");
gp_node03756->SetName("W Health Outcomes");
gp_node03756->SetInternal(false);
gp_node03756->SetInitNumInstances(11);
gp_node03756->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node03759=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03759->SetID("node03759");
gp_node03759->SetName("Wealth\nVulnerability");
gp_node03759->SetInternal(false);
gp_node03759->SetExpr("");
gp_node03759->SetTableID("");
gp_node03759->SetExpectsFile(true);
gp_node03759->SetInitOnly(true);
gp_node03756->AddVariable(gp_node03759);

gp_node03743->AddSubmodel(gp_node03756);

gp_node03595->AddSubmodel(gp_node03743);

gp_root->AddSubmodel(gp_node03595);

gp_node03606=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03606->SetID("node03606");
gp_node03606->SetName("Load GCC scenario outputs");
gp_node03606->SetInternal(false);
gp_node03606->SetInitNumInstances(12);
gp_node03606->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node03760=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03760->SetID("node03760");
gp_node03760->SetName("Years");
gp_node03760->SetInternal(false);
gp_node03760->SetInitNumInstances(100);
gp_node03760->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node03765=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03765->SetID("node03765");
gp_node03765->SetName("Precip");
gp_node03765->SetInternal(false);
gp_node03765->SetExpr("");
gp_node03765->SetTableID("");
gp_node03765->SetExpectsFile(true);
gp_node03765->SetInitOnly(true);
gp_node03760->AddVariable(gp_node03765);

gp_node03766=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03766->SetID("node03766");
gp_node03766->SetName("Humidity");
gp_node03766->SetInternal(false);
gp_node03766->SetExpr("");
gp_node03766->SetTableID("");
gp_node03766->SetExpectsFile(true);
gp_node03766->SetInitOnly(true);
gp_node03760->AddVariable(gp_node03766);

gp_node03767=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03767->SetID("node03767");
gp_node03767->SetName("Temp");
gp_node03767->SetInternal(false);
gp_node03767->SetExpr("");
gp_node03767->SetTableID("");
gp_node03767->SetExpectsFile(true);
gp_node03767->SetInitOnly(true);
gp_node03760->AddVariable(gp_node03767);

gp_node03768=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03768->SetID("node03768");
gp_node03768->SetName("Wind");
gp_node03768->SetInternal(false);
gp_node03768->SetExpr("0.0");
gp_node03768->SetTableID("");
gp_node03768->SetExpectsFile(false);
gp_node03768->SetInitOnly(true);
gp_node03760->AddVariable(gp_node03768);

gp_node03606->AddSubmodel(gp_node03760);

gp_root->AddSubmodel(gp_node03606);

gp_node03608=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03608->SetID("node03608");
gp_node03608->SetName("Yearly Data");
gp_node03608->SetInternal(false);
gp_node03608->SetInitNumInstances(365);
gp_node03608->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node03776=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03776->SetID("node03776");
gp_node03776->SetName("Weather");
gp_node03776->SetInternal(false);
gp_node03776->SetInitNumInstances(40);
gp_node03776->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node03781=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03781->SetID("node03781");
gp_node03781->SetName("Windspeed");
gp_node03781->SetInternal(false);
gp_node03781->SetExpr("0.0");
gp_node03781->SetTableID("");
gp_node03781->SetExpectsFile(false);
gp_node03781->SetInitOnly(true);
gp_node03776->AddVariable(gp_node03781);

gp_node03783=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03783->SetID("node03783");
gp_node03783->SetName("Temp_C");
gp_node03783->SetInternal(false);
gp_node03783->SetExpr("");
gp_node03783->SetTableID("");
gp_node03783->SetAskVal(true);
gp_node03783->SetMinValue(1);
gp_node03783->SetMaxValue(365);
gp_node03783->SetExpectsFile(true);
gp_node03783->SetInitOnly(true);
gp_node03776->AddVariable(gp_node03783);

gp_node03784=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03784->SetID("node03784");
gp_node03784->SetName("Humidity");
gp_node03784->SetInternal(false);
gp_node03784->SetExpr("");
gp_node03784->SetTableID("");
gp_node03784->SetAskVal(true);
gp_node03784->SetMinValue(1);
gp_node03784->SetMaxValue(365);
gp_node03784->SetExpectsFile(true);
gp_node03784->SetInitOnly(true);
gp_node03776->AddVariable(gp_node03784);

gp_node03786=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03786->SetID("node03786");
gp_node03786->SetName("PREC");
gp_node03786->SetInternal(false);
gp_node03786->SetExpr("");
gp_node03786->SetTableID("");
gp_node03786->SetExpectsFile(true);
gp_node03786->SetInitOnly(true);
gp_node03776->AddVariable(gp_node03786);

gp_node03608->AddSubmodel(gp_node03776);

gp_root->AddSubmodel(gp_node03608);

gp_node03611=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03611->SetID("node03611");
gp_node03611->SetName("Pollutant\nattributes");
gp_node03611->SetInternal(false);
gp_node03611->SetInitNumInstances(6);
gp_node03611->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node03788=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03788->SetID("node03788");
gp_node03788->SetName(" \npollutant\nremoval\nrate");
gp_node03788->SetInternal(false);
gp_node03788->SetExpr("element([ 9812.19, 2.47466, 10.017, 38.7003, 5.47233, 25.8955],index(1.0))/365.0");
gp_node03788->SetTableID("");
gp_node03788->SetExpectsFile(false);
gp_node03788->SetInitOnly(false);
gp_node03611->AddVariable(gp_node03788);

gp_node03790=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03790->SetID("node03790");
gp_node03790->SetName("pollutant\nremoval unit value");
gp_node03790->SetInternal(false);
gp_node03790->SetExpr("element([ 5.64, 1276.4, 8986.57, 8986.57, 2199.92, 6000.11],index(1.0))");
gp_node03790->SetTableID("");
gp_node03790->SetExpectsFile(false);
gp_node03790->SetInitOnly(false);
gp_node03611->AddVariable(gp_node03790);

gp_node03792=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03792->SetID("node03792");
gp_node03792->SetName("Total pollutant\nRemoval\nValue");
gp_node03792->SetInternal(false);
gp_node03792->SetExpr("element(sum([[ AustinModel_determ.Decision_Units.Air_pollution.Pollutant_removal ]])*AustinModel_determ.Pollutant_attributes.pollutant_removal_unit_value/2000.0,index(1.0))");
gp_node03792->SetTableID("");
gp_node03792->SetExpectsFile(false);
gp_node03792->SetInitOnly(false);
gp_node03611->AddVariable(gp_node03792);

gp_node03794=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03794->SetID("node03794");
gp_node03794->SetName("Wind sensitive");
gp_node03794->SetInternal(false);
gp_node03794->SetExpr("0.0");
gp_node03794->SetTableID("");
gp_node03794->SetAsInt(true);
gp_node03794->SetAskVal(true);
gp_node03794->SetMinValue(0);
gp_node03794->SetMaxValue(1);
gp_node03794->SetExpectsFile(false);
gp_node03794->SetInitOnly(true);
gp_node03611->AddVariable(gp_node03794);

gp_node03797=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03797->SetID("node03797");
gp_node03797->SetName("Global\nconcentration");
gp_node03797->SetInternal(false);
gp_node03797->SetExpr(" 0.35");
gp_node03797->SetTableID("");
gp_node03797->SetExpectsFile(false);
gp_node03797->SetInitOnly(true);
gp_node03611->AddVariable(gp_node03797);

gp_node03800=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03800->SetID("node03800");
gp_node03800->SetName("diffusion rt");
gp_node03800->SetInternal(false);
gp_node03800->SetExpr("1.0");
gp_node03800->SetTableID("");
gp_node03800->SetExpectsFile(false);
gp_node03800->SetInitOnly(true);
gp_node03611->AddVariable(gp_node03800);

gp_root->AddSubmodel(gp_node03611);

gp_node03627=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03627->SetID("node03627");
gp_node03627->SetName("Land Cover\nAttributes");
gp_node03627->SetInternal(false);
gp_node03627->SetInitNumInstances(15);
gp_node03627->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node03804=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03804->SetID("node03804");
gp_node03804->SetName("Temperature\nbuffer capacity");
gp_node03804->SetInternal(false);
gp_node03804->SetExpr("(if index(1.0)==1.0 then  AustinModel_determ.Air_T_reduction_by_water  else 0.0 end)");
gp_node03804->SetTableID("");
gp_node03804->SetExpectsFile(false);
gp_node03804->SetInitOnly(false);
gp_node03627->AddVariable(gp_node03804);

gp_node03809=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03809->SetID("node03809");
gp_node03809->SetName("Effect on wind");
gp_node03809->SetInternal(false);
gp_node03809->SetExpr("0.0");
gp_node03809->SetTableID("");
gp_node03809->SetExpectsFile(false);
gp_node03809->SetInitOnly(true);
gp_node03627->AddVariable(gp_node03809);

gp_node03812=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03812->SetID("node03812");
gp_node03812->SetName("Pervious");
gp_node03812->SetInternal(false);
gp_node03812->SetExpr("element([0.0, 0.9, 0.65, 0.35, 0.1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0],index(1.0))");
gp_node03812->SetTableID("");
gp_node03812->SetExpectsFile(false);
gp_node03812->SetInitOnly(false);
gp_node03627->AddVariable(gp_node03812);

gp_node03814=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03814->SetID("node03814");
gp_node03814->SetName("Tree cover");
gp_node03814->SetInternal(false);
gp_node03814->SetExpr("");
gp_node03814->SetTableID("");
gp_node03814->SetExpectsFile(true);
gp_node03814->SetInitOnly(true);
gp_node03627->AddVariable(gp_node03814);

gp_node03817=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03817->SetID("node03817");
gp_node03817->SetName("Air T slope");
gp_node03817->SetInternal(false);
gp_node03817->SetExpr(" 0.5309");
gp_node03817->SetTableID("");
gp_node03817->SetExpectsFile(true);
gp_node03817->SetInitOnly(true);
gp_node03627->AddVariable(gp_node03817);

gp_node03820=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03820->SetID("node03820");
gp_node03820->SetName("NDVI");
gp_node03820->SetInternal(false);
gp_node03820->SetExpr("element([0.0, AustinModel_determ.Land_Cover_Attributes.Tree_cover *element([ AustinModel_determ.NDVI_observed.NDVI ],2.0)+( AustinModel_determ.Land_Cover_Attributes.Pervious - AustinModel_determ.Land_Cover_Attributes.Tree_cover )*element([ AustinModel_determ.NDVI_observed.NDVI ],5.0), AustinModel_determ.Land_Cover_Attributes.Tree_cover *element([ AustinModel_determ.NDVI_observed.NDVI ],2.0)+( AustinModel_determ.Land_Cover_Attributes.Pervious - AustinModel_determ.Land_Cover_Attributes.Tree_cover )*element([ AustinModel_determ.NDVI_observed.NDVI ],5.0), AustinModel_determ.Land_Cover_Attributes.Tree_cover *element([ AustinModel_determ.NDVI_observed.NDVI ],2.0)+( AustinModel_determ.Land_Cover_Attributes.Pervious - AustinModel_determ.Land_Cover_Attributes.Tree_cover )*element([ AustinModel_determ.NDVI_observed.NDVI ],5.0), AustinModel_determ.Land_Cover_Attributes.Tree_cover *element([ AustinModel_determ.NDVI_observed.NDVI ],2.0)+( AustinModel_determ.Land_Cover_Attributes.Pervious - AustinModel_determ.Land_Cover_Attributes.Tree_cover )*element([ AustinModel_determ.NDVI_observed.NDVI ],5.0), 0.06,element([ AustinModel_determ.NDVI_observed.NDVI ],1.0),element([ AustinModel_determ.NDVI_observed.NDVI ],2.0),element([ AustinModel_determ.NDVI_observed.NDVI ],3.0),element([ AustinModel_determ.NDVI_observed.NDVI ],4.0),element([ AustinModel_determ.NDVI_observed.NDVI ],5.0),element([ AustinModel_determ.NDVI_observed.NDVI ],6.0),element([ AustinModel_determ.NDVI_observed.NDVI ],7.0),element([ AustinModel_determ.NDVI_observed.NDVI ],8.0),element([ AustinModel_determ.NDVI_observed.NDVI ],9.0)],index(1.0))");
gp_node03820->SetTableID("");
gp_node03820->SetAskVal(true);
gp_node03820->SetMinValue(0);
gp_node03820->SetMaxValue(1);
gp_node03820->SetExpectsFile(true);
gp_node03820->SetInitOnly(false);
gp_node03627->AddVariable(gp_node03820);

gp_node03821=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03821->SetID("node03821");
gp_node03821->SetName("Boundary Layer\nHeat Conductance");
gp_node03821->SetInternal(false);
gp_node03821->SetExpr("");
gp_node03821->SetTableID("");
gp_node03821->SetExpectsFile(true);
gp_node03821->SetInitOnly(true);
gp_node03627->AddVariable(gp_node03821);

gp_node03823=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03823->SetID("node03823");
gp_node03823->SetName("Air T intercept");
gp_node03823->SetInternal(false);
gp_node03823->SetExpr(" 10.0275");
gp_node03823->SetTableID("");
gp_node03823->SetExpectsFile(true);
gp_node03823->SetInitOnly(true);
gp_node03627->AddVariable(gp_node03823);

gp_node03973=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03973->SetID("node03973");
gp_node03973->SetName("Net radiant\nEnergy");
gp_node03973->SetInternal(false);
gp_node03973->SetExpr(" AustinModel_determ.Land_Cover_Attributes.Boundary_Layer_Heat_Conductance * 29.17 *( AustinModel_determ.Bare_Soil_Temperature - AustinModel_determ.Air_T )");
gp_node03973->SetTableID("");
gp_node03973->SetExpectsFile(false);
gp_node03973->SetInitOnly(false);
gp_node03627->AddVariable(gp_node03973);

gp_root->AddSubmodel(gp_node03627);

gp_node03642=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03642->SetID("node03642");
gp_node03642->SetName("Displays");
gp_node03642->SetInternal(false);
gp_node03642->SetInitNumInstances(696);
gp_node03642->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node03825=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03825->SetID("node03825");
gp_node03825->SetName("Heat Stress Index");
gp_node03825->SetInternal(false);
gp_node03825->SetExpr("");
gp_node03825->SetTableID("");
gp_node03825->SetExpectsFile(true);
gp_node03825->SetInitOnly(true);
gp_node03642->AddVariable(gp_node03825);

gp_node03826=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03826->SetID("node03826");
gp_node03826->SetName("Wealth");
gp_node03826->SetInternal(false);
gp_node03826->SetExpr("element([ AustinModel_determ.Decision_Units.Social_Fabric.Wealth ],index(1.0))");
gp_node03826->SetTableID("");
gp_node03826->SetExpectsFile(false);
gp_node03826->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03826);

gp_node03828=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03828->SetID("node03828");
gp_node03828->SetName("Rural");
gp_node03828->SetInternal(false);
gp_node03828->SetExpr("element([ AustinModel_determ.Decision_Units.Social_Fabric.Rural_ratio_ ],index(1.0))");
gp_node03828->SetTableID("");
gp_node03828->SetExpectsFile(false);
gp_node03828->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03828);

gp_node03830=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03830->SetID("node03830");
gp_node03830->SetName("Empl");
gp_node03830->SetInternal(false);
gp_node03830->SetExpr("element([ AustinModel_determ.Decision_Units.Social_Fabric.Employed_ratio ],index(1.0))");
gp_node03830->SetTableID("");
gp_node03830->SetExpectsFile(false);
gp_node03830->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03830);

gp_node03834=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03834->SetID("node03834");
gp_node03834->SetName("Children");
gp_node03834->SetInternal(false);
gp_node03834->SetExpr("sum(element(element([[ AustinModel_determ.Decision_Units.Social_Fabric.Race_Eth_Gen_Age ]],index(1.0)),[ AustinModel_determ.Children ]))");
gp_node03834->SetTableID("");
gp_node03834->SetExpectsFile(false);
gp_node03834->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03834);

gp_node03836=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03836->SetID("node03836");
gp_node03836->SetName("Elderly");
gp_node03836->SetInternal(false);
gp_node03836->SetExpr("sum(element(element([[ AustinModel_determ.Decision_Units.Social_Fabric.Race_Eth_Gen_Age ]],index(1.0)),[ AustinModel_determ.Elderly ]))");
gp_node03836->SetTableID("");
gp_node03836->SetExpectsFile(false);
gp_node03836->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03836);

gp_node03841=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03841->SetID("node03841");
gp_node03841->SetName("Water\ntemperature\nreduction");
gp_node03841->SetInternal(false);
gp_node03841->SetExpr("element(element([[AustinModel_determ.Decision_Units.Land_Covers.temperature_reduction]],index(1.0)),1.0)");
gp_node03841->SetTableID("");
gp_node03841->SetExpectsFile(false);
gp_node03841->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03841);

gp_node03844=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03844->SetID("node03844");
gp_node03844->SetName("Complaints Index");
gp_node03844->SetInternal(false);
gp_node03844->SetExpr("element([AustinModel_determ.Displays.M_estimates.cumulative_morbidity],2.0)");
gp_node03844->SetTableID("");
gp_node03844->SetExpectsFile(false);
gp_node03844->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03844);

gp_node03848=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03848->SetID("node03848");
gp_node03848->SetName("EMS Index");
gp_node03848->SetInternal(false);
gp_node03848->SetExpr("element([AustinModel_determ.Displays.M_estimates.cumulative_morbidity],1.0)");
gp_node03848->SetTableID("");
gp_node03848->SetExpectsFile(false);
gp_node03848->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03848);

gp_node03851=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03851->SetID("node03851");
gp_node03851->SetName("Morbidity est");
gp_node03851->SetInternal(false);
gp_node03851->SetExpr("element([ AustinModel_determ.Displays.M_estimates.Morbidity ], AustinModel_determ.Choose_Heat_Mortality_Index )");
gp_node03851->SetTableID("");
gp_node03851->SetExpectsFile(false);
gp_node03851->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03851);

gp_node03853=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03853->SetID("node03853");
gp_node03853->SetName("Social Fabric Display");
gp_node03853->SetInternal(false);
gp_node03853->SetExpr("element([ AustinModel_determ.Displays.Children , AustinModel_determ.Displays.Elderly , AustinModel_determ.Displays.Non_white_pop , AustinModel_determ.Displays.Wealth , AustinModel_determ.Displays.Empl ], AustinModel_determ.Choice_of_Social_Fabric_Indicator )");
gp_node03853->SetTableID("");
gp_node03853->SetExpectsFile(false);
gp_node03853->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03853);

gp_node03856=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03856->SetID("node03856");
gp_node03856->SetName("Tree cover ratio");
gp_node03856->SetInternal(false);
gp_node03856->SetExpr("element([ AustinModel_determ.Decision_Units.Tree_Cover ],index(1.0))");
gp_node03856->SetTableID("");
gp_node03856->SetExpectsFile(false);
gp_node03856->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03856);

gp_node03862=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03862->SetID("node03862");
gp_node03862->SetName("Non white pop");
gp_node03862->SetInternal(false);
gp_node03862->SetExpr("sum(element(element([[ AustinModel_determ.Decision_Units.Social_Fabric.Race_Eth_Gen_Age ]],index(1.0)),[ AustinModel_determ.None_White ]))/sum(sum([[ AustinModel_determ.Decision_Units.Social_Fabric.Race_Eth_Gen_Age ]]))");
gp_node03862->SetTableID("");
gp_node03862->SetExpectsFile(false);
gp_node03862->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03862);

gp_node03863=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03863->SetID("node03863");
gp_node03863->SetName("NDVI");
gp_node03863->SetInternal(false);
gp_node03863->SetExpr("element([ AustinModel_determ.Decision_Units.NDVI ],index(1.0))");
gp_node03863->SetTableID("");
gp_node03863->SetExpectsFile(false);
gp_node03863->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03863);

gp_node03869=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03869->SetID("node03869");
gp_node03869->SetName("Air temperatures deviation");
gp_node03869->SetInternal(false);
gp_node03869->SetExpr("element([ AustinModel_determ.Decision_Units.Average_air_temperatures ],index(1.0))-sum([ AustinModel_determ.Decision_Units.Average_air_temperatures ])/count([ AustinModel_determ.Decision_Units.Average_air_temperatures ])");
gp_node03869->SetTableID("");
gp_node03869->SetExpectsFile(false);
gp_node03869->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03869);

gp_node03871=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03871->SetID("node03871");
gp_node03871->SetName("Heat Wave");
gp_node03871->SetInternal(false);
gp_node03871->SetExpr("element([ AustinModel_determ.Decision_Units.Heat_Wave ],index(1.0))");
gp_node03871->SetTableID("");
gp_node03871->SetExpectsFile(false);
gp_node03871->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03871);

gp_node03872=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03872->SetID("node03872");
gp_node03872->SetName("blgr ID");
gp_node03872->SetInternal(false);
gp_node03872->SetExpr("element([ AustinModel_determ.Blgr_ids ],index(1.0))");
gp_node03872->SetTableID("");
gp_node03872->SetExpectsFile(false);
gp_node03872->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03872);

gp_node03877=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03877->SetID("node03877");
gp_node03877->SetName("Dominant LU");
gp_node03877->SetInternal(false);
gp_node03877->SetExpr("element([AustinModel_determ.Decision_Units.dominant_lu],index(1.0))");
gp_node03877->SetTableID("");
gp_node03877->SetExpectsFile(false);
gp_node03877->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03877);

gp_node03880=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03880->SetID("node03880");
gp_node03880->SetName("Surface Temperature");
gp_node03880->SetInternal(false);
gp_node03880->SetExpr("element([ AustinModel_determ.Decision_Units.Av_Can_T ],index(1.0))");
gp_node03880->SetTableID("");
gp_node03880->SetExpectsFile(false);
gp_node03880->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03880);

gp_node03883=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03883->SetID("node03883");
gp_node03883->SetName("Landcover");
gp_node03883->SetInternal(false);
gp_node03883->SetExpr("element([ AustinModel_determ.Decision_Units.Land_Cover ],index(1.0))");
gp_node03883->SetTableID("");
gp_node03883->SetExpectsFile(false);
gp_node03883->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03883);

gp_node03893=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03893->SetID("node03893");
gp_node03893->SetName("Bare ground T");
gp_node03893->SetInternal(false);
gp_node03893->SetExpr("element([ AustinModel_determ.Decision_Units.Bare_Ground_T ],index(1.0))");
gp_node03893->SetTableID("");
gp_node03893->SetExpectsFile(false);
gp_node03893->SetInitOnly(false);
gp_node03642->AddVariable(gp_node03893);

gp_node03847=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03847->SetID("node03847");
gp_node03847->SetName("M estimates");
gp_node03847->SetInternal(false);
gp_node03847->SetInitNumInstances(5);
gp_node03847->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node03904=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03904->SetID("node03904");
gp_node03904->SetName("Morbidity");
gp_node03904->SetInternal(false);
gp_node03904->SetExpr("element([element([ AustinModel_determ.Decision_Units.EMS ],index(2.0)),element([ AustinModel_determ.Decision_Units.Complaints ],index(2.0)),element([ AustinModel_determ.Decision_Units.Mortality_rt ],index(2.0)),element([ AustinModel_determ.Decision_Units.Num_Mortality ],index(2.0)),element([ AustinModel_determ.Decision_Units.Cat_Mortality_rt ],index(2.0))],index(1.0))");
gp_node03904->SetTableID("");
gp_node03904->SetExpectsFile(false);
gp_node03904->SetInitOnly(false);
gp_node03847->AddVariable(gp_node03904);

gp_node03896=(StateVar*)TestModelBm->GenModelObject(OME_STATEVAR);
gp_node03896->SetID("node03896");
gp_node03896->SetName("cumulative morbidity");
gp_node03896->SetInternal(false);
gp_node03896->SetExpr("0.0");
gp_node03896->SetTableID("");
gp_node03896->SetInitCondition(0);
gp_node03847->AddStateVar(gp_node03896);

gp_arc03930=(Flow*)TestModelBm->GenModelObject(OME_FLOW);
gp_arc03930->SetID("arc03930");
gp_arc03930->SetName("add mortalities rt");
gp_arc03930->SetInternal(false);
gp_arc03930->SetExpr(" AustinModel_determ.Displays.M_estimates.Morbidity -(if AustinModel_determ.JDAY ==365.0 then AustinModel_determ.Displays.M_estimates.cumulative_morbidity else 0.0 end)");
gp_arc03930->SetTableID("");
gp_arc03930->SetTargetID("node03896");
gp_node03847->AddFlow(gp_arc03930);

gp_node03642->AddSubmodel(gp_node03847);

gp_root->AddSubmodel(gp_node03642);

gp_node03644=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03644->SetID("node03644");
gp_node03644->SetName("Decision Units");
gp_node03644->SetInternal(false);
gp_node03644->SetInitNumInstances(696);
gp_node03644->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node00073=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node00073->SetID("node00073");
gp_node00073->SetName("Cat\nMortality rt");
gp_node00073->SetInternal(false);
gp_node00073->SetExpr("AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.heatwave*( 0.0374+(if AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.Count_Heatwave >1.0 then  0.0504 else  0.0265 end)+ AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.Intensity / 1.8 * 0.249 + AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.Period * 0.038)");
gp_node00073->SetTableID("");
gp_node00073->SetExpectsFile(false);
gp_node00073->SetInitOnly(false);
gp_node03644->AddVariable(gp_node00073);

gp_node00074=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node00074->SetID("node00074");
gp_node00074->SetName("Mortality");
gp_node00074->SetInternal(false);
gp_node00074->SetExpr("element([ AustinModel_determ.Decision_Units.Mortality_rt , AustinModel_determ.Decision_Units.Num_Mortality , AustinModel_determ.Decision_Units.Cat_Mortality_rt ], AustinModel_determ.Mortality_Choice )");
gp_node00074->SetTableID("");
gp_node00074->SetExpectsFile(false);
gp_node00074->SetInitOnly(false);
gp_node03644->AddVariable(gp_node00074);

gp_node00077=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node00077->SetID("node00077");
gp_node00077->SetName("Num\nMortality");
gp_node00077->SetInternal(false);
gp_node00077->SetExpr(" AustinModel_determ.Decision_Units.Heat_Wave_Characteristics._Heat_effect * AustinModel_determ.Numeric_Heat_effect_mortality ");
gp_node00077->SetTableID("");
gp_node00077->SetExpectsFile(false);
gp_node00077->SetInitOnly(false);
gp_node03644->AddVariable(gp_node00077);

gp_node00079=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node00079->SetID("node00079");
gp_node00079->SetName("Mortality rt");
gp_node00079->SetInternal(false);
gp_node00079->SetExpr("sum(sum([ AustinModel_determ.Decision_Units.Social_Fabric.Race_Eth_Gen_Age ]*sum([ AustinModel_determ.Decision_Units.Hazards ]*[[[ AustinModel_determ.Hazard_Risk_Factors.Race_Eth_Gen_Age.E_Health_Outcomes.Race_Eth_Gen_Age_Vulnerability ]]])))+sum(element(sum([ AustinModel_determ.Decision_Units.Hazards ]*[[[ AustinModel_determ.Hazard_Risk_Factors.Income_groups.W_Health_Outcomes.Wealth_Vulnerability ]]]), AustinModel_determ.Decision_Units.Social_Fabric.Wealth_position ))+sum(sum([ AustinModel_determ.Decision_Units.Social_Fabric.Life_Style ]*sum([ AustinModel_determ.Decision_Units.Hazards ]*[[[ AustinModel_determ.Hazard_Risk_Factors.Life_Style.L_Health_Outcomes.Life_Style_Vulnerability ]]])))+sum(sum([ AustinModel_determ.Decision_Units.Social_Fabric.Employment ]*sum([ AustinModel_determ.Decision_Units.Hazards ]*[[[ AustinModel_determ.Hazard_Risk_Factors.Employment.E_Health_Outcomes.Employment_Vulnerability ]]])))");
gp_node00079->SetTableID("");
gp_node00079->SetExpectsFile(false);
gp_node00079->SetInitOnly(false);
gp_node03644->AddVariable(gp_node00079);

gp_node00230=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node00230->SetID("node00230");
gp_node00230->SetName("DU Evap contribution");
gp_node00230->SetInternal(false);
gp_node00230->SetExpr(" AustinModel_determ.Decision_Units.Area / AustinModel_determ.Total_area * AustinModel_determ.Decision_Units.Montly_Sum ");
gp_node00230->SetTableID("");
gp_node00230->SetExpectsFile(false);
gp_node00230->SetInitOnly(false);
gp_node03644->AddVariable(gp_node00230);

gp_node03927=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03927->SetID("node03927");
gp_node03927->SetName("population");
gp_node03927->SetInternal(false);
gp_node03927->SetExpr("");
gp_node03927->SetTableID("");
gp_node03927->SetExpectsFile(true);
gp_node03927->SetInitOnly(true);
gp_node03644->AddVariable(gp_node03927);

gp_node03929=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03929->SetID("node03929");
gp_node03929->SetName("DU Evap");
gp_node03929->SetInternal(false);
gp_node03929->SetExpr("sum([AustinModel_determ.Decision_Units.Land_Covers.evaporation]*[ AustinModel_determ.Decision_Units.Land_Covers.LandCover_Proportions ])");
gp_node03929->SetTableID("");
gp_node03929->SetExpectsFile(false);
gp_node03929->SetInitOnly(false);
gp_node03644->AddVariable(gp_node03929);

gp_node03934=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03934->SetID("node03934");
gp_node03934->SetName("Tree Cover");
gp_node03934->SetInternal(false);
gp_node03934->SetExpr("sum([ AustinModel_determ.Decision_Units.Land_Covers.Tree_cover_area ])/ AustinModel_determ.Decision_Units.Area ");
gp_node03934->SetTableID("");
gp_node03934->SetExpectsFile(false);
gp_node03934->SetInitOnly(false);
gp_node03644->AddVariable(gp_node03934);

gp_node03939=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03939->SetID("node03939");
gp_node03939->SetName("Air stagnation");
gp_node03939->SetInternal(false);
gp_node03939->SetExpr("(if AustinModel_determ.Decision_Units.Windspeed <13.0 then 1.0 else 0.0 end)");
gp_node03939->SetTableID("");
gp_node03939->SetExpectsFile(false);
gp_node03939->SetInitOnly(false);
gp_node03644->AddVariable(gp_node03939);

gp_node03950=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03950->SetID("node03950");
gp_node03950->SetName("EMS");
gp_node03950->SetInternal(false);
gp_node03950->SetExpr("max(0.0, -8.262408+ 0.3614094 * AustinModel_determ.Decision_Units.Av_Can_T )* AustinModel_determ.Decision_Units.Population_modifier ");
gp_node03950->SetTableID("");
gp_node03950->SetExpectsFile(false);
gp_node03950->SetInitOnly(false);
gp_node03644->AddVariable(gp_node03950);

gp_node03952=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03952->SetID("node03952");
gp_node03952->SetName("Complaints");
gp_node03952->SetInternal(false);
gp_node03952->SetExpr("max(0.0, -8.031008+ 0.5187364 * AustinModel_determ.Decision_Units.Av_Can_T )* AustinModel_determ.Decision_Units.Population_modifier ");
gp_node03952->SetTableID("");
gp_node03952->SetExpectsFile(false);
gp_node03952->SetInitOnly(false);
gp_node03644->AddVariable(gp_node03952);

gp_node03954=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03954->SetID("node03954");
gp_node03954->SetName("T Air pollution");
gp_node03954->SetInternal(false);
gp_node03954->SetExpr("sum([ AustinModel_determ.Decision_Units.Air_pollution.Pollution_level ])");
gp_node03954->SetTableID("");
gp_node03954->SetAskVal(true);
gp_node03954->SetMinValue(0);
gp_node03954->SetMaxValue(1);
gp_node03954->SetExpectsFile(false);
gp_node03954->SetInitOnly(false);
gp_node03644->AddVariable(gp_node03954);

gp_node03958=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03958->SetID("node03958");
gp_node03958->SetName("NDVI");
gp_node03958->SetInternal(false);
gp_node03958->SetExpr("sum([AustinModel_determ.Decision_Units.Land_Covers.weighted_NDVI])");
gp_node03958->SetTableID("");
gp_node03958->SetExpectsFile(false);
gp_node03958->SetInitOnly(false);
gp_node03644->AddVariable(gp_node03958);

gp_node03963=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03963->SetID("node03963");
gp_node03963->SetName("Est Morbidity");
gp_node03963->SetInternal(false);
gp_node03963->SetExpr(" 0.05 *sum(sum([ AustinModel_determ.Decision_Units.Social_Fabric.Race_Eth_Gen_Age ]*sum([ AustinModel_determ.Decision_Units.Hazards ]*[[[ AustinModel_determ.Hazard_Risk_Factors.Race_Eth_Gen_Age.E_Health_Outcomes.Race_Eth_Gen_Age_Vulnerability ]]])))+sum(element(sum([ AustinModel_determ.Decision_Units.Hazards ]*[[[ AustinModel_determ.Hazard_Risk_Factors.Income_groups.W_Health_Outcomes.Wealth_Vulnerability ]]]), AustinModel_determ.Decision_Units.Social_Fabric.Wealth_position ))+sum(sum([ AustinModel_determ.Decision_Units.Social_Fabric.Life_Style ]*sum([ AustinModel_determ.Decision_Units.Hazards ]*[[[ AustinModel_determ.Hazard_Risk_Factors.Life_Style.L_Health_Outcomes.Life_Style_Vulnerability ]]])))+sum(sum([ AustinModel_determ.Decision_Units.Social_Fabric.Employment ]*sum([ AustinModel_determ.Decision_Units.Hazards ]*[[[ AustinModel_determ.Hazard_Risk_Factors.Employment.E_Health_Outcomes.Employment_Vulnerability ]]])))");
gp_node03963->SetTableID("");
gp_node03963->SetExpectsFile(false);
gp_node03963->SetInitOnly(false);
gp_node03644->AddVariable(gp_node03963);

gp_node03965=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03965->SetID("node03965");
gp_node03965->SetName("Heat Wave");
gp_node03965->SetInternal(false);
gp_node03965->SetExpr("element([ AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.A_and_B_HW , AustinModel_determ.Decision_Units.Heat_Wave_Characteristics._Heat_effect ], AustinModel_determ.Choose_Heat_wave_definition )");
gp_node03965->SetTableID("");
gp_node03965->SetExpectsFile(false);
gp_node03965->SetInitOnly(false);
gp_node03644->AddVariable(gp_node03965);

gp_node03970=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03970->SetID("node03970");
gp_node03970->SetName("Area");
gp_node03970->SetInternal(false);
gp_node03970->SetExpr("");
gp_node03970->SetTableID("");
gp_node03970->SetExpectsFile(true);
gp_node03970->SetInitOnly(true);
gp_node03644->AddVariable(gp_node03970);

gp_node03971=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03971->SetID("node03971");
gp_node03971->SetName("dominant lu");
gp_node03971->SetInternal(false);
gp_node03971->SetExpr("posgreatest([ AustinModel_determ.Decision_Units.Land_Covers.LandCover_Proportions ])");
gp_node03971->SetTableID("");
gp_node03971->SetExpectsFile(false);
gp_node03971->SetInitOnly(false);
gp_node03644->AddVariable(gp_node03971);

gp_node03975=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03975->SetID("node03975");
gp_node03975->SetName("Average\nair temperatures");
gp_node03975->SetInternal(false);
gp_node03975->SetExpr("sum([ AustinModel_determ.Decision_Units.Land_Covers.Air_temp ]*[ AustinModel_determ.Decision_Units.Land_Covers.LandCover_Proportions ])");
gp_node03975->SetTableID("");
gp_node03975->SetExpectsFile(false);
gp_node03975->SetInitOnly(false);
gp_node03644->AddVariable(gp_node03975);

gp_node03977=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03977->SetID("node03977");
gp_node03977->SetName("Forest T");
gp_node03977->SetInternal(false);
gp_node03977->SetExpr("element([ AustinModel_determ.Decision_Units.Land_Covers.Canopy_temp ],7.0)");
gp_node03977->SetTableID("");
gp_node03977->SetExpectsFile(false);
gp_node03977->SetInitOnly(false);
gp_node03644->AddVariable(gp_node03977);

gp_node03979=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03979->SetID("node03979");
gp_node03979->SetName("Av Can T");
gp_node03979->SetInternal(false);
gp_node03979->SetExpr("sum([ AustinModel_determ.Decision_Units.Land_Covers.Canopy_temp ]*[ AustinModel_determ.Decision_Units.Land_Covers.LandCover_Proportions ])");
gp_node03979->SetTableID("");
gp_node03979->SetExpectsFile(false);
gp_node03979->SetInitOnly(false);
gp_node03644->AddVariable(gp_node03979);

gp_node03983=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03983->SetID("node03983");
gp_node03983->SetName("Land\nCover");
gp_node03983->SetInternal(false);
gp_node03983->SetExpr("element([ AustinModel_determ.Decision_Units.Land_Covers.LandCover_Proportions ], AustinModel_determ.Choose_LandCover )");
gp_node03983->SetTableID("");
gp_node03983->SetExpectsFile(false);
gp_node03983->SetInitOnly(false);
gp_node03644->AddVariable(gp_node03983);

gp_node03990=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node03990->SetID("node03990");
gp_node03990->SetName("Bare\nGround T");
gp_node03990->SetInternal(false);
gp_node03990->SetExpr("element([ AustinModel_determ.Decision_Units.Land_Covers.Canopy_temp ],6.0)");
gp_node03990->SetTableID("");
gp_node03990->SetExpectsFile(false);
gp_node03990->SetInitOnly(false);
gp_node03644->AddVariable(gp_node03990);

gp_node04000=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04000->SetID("node04000");
gp_node04000->SetName("Population modifier");
gp_node04000->SetInternal(false);
gp_node04000->SetExpr("AustinModel_determ.Decision_Units.population/AustinModel_determ.total_population");
gp_node04000->SetTableID("");
gp_node04000->SetExpectsFile(false);
gp_node04000->SetInitOnly(false);
gp_node03644->AddVariable(gp_node04000);

gp_node04004=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04004->SetID("node04004");
gp_node04004->SetName("Windspeed");
gp_node04004->SetInternal(false);
gp_node04004->SetExpr(" AustinModel_determ.Windspeed -sum([ AustinModel_determ.Decision_Units.Land_Covers.Effect_on_wind ])");
gp_node04004->SetTableID("");
gp_node04004->SetExpectsFile(false);
gp_node04004->SetInitOnly(false);
gp_node03644->AddVariable(gp_node04004);

gp_node03961=(VarArray*)TestModelBm->GenModelObject(OME_VARARRAY);
gp_node03961->SetID("node03961");
gp_node03961->SetName("Hazards");
gp_node03961->SetInternal(false);
gp_node03961->SetExpr("[ AustinModel_determ.Decision_Units.Heat_Wave ,0.0, AustinModel_determ.Decision_Units.T_Air_pollution ,0.0,0.0,0.0,0.0,0.0,0.0,1.0]");
gp_node03961->SetTableID("");
gp_node03961->SetExpectsFile(false);
gp_node03961->SetInitOnly(false);
IndVec node03961_dims=IndVec(1);
node03961_dims[0]=10;
gp_node03961->OverrideDims(node03961_dims);
gp_node03644->AddVariable(gp_node03961);

gp_node03613=(StateVar*)TestModelBm->GenModelObject(OME_STATEVAR);
gp_node03613->SetID("node03613");
gp_node03613->SetName("Monthly Evap");
gp_node03613->SetInternal(false);
gp_node03613->SetExpr("0.0");
gp_node03613->SetTableID("");
gp_node03613->SetInitCondition(0);
gp_node03644->AddStateVar(gp_node03613);

gp_arc04227=(Flow*)TestModelBm->GenModelObject(OME_FLOW);
gp_arc04227->SetID("arc04227");
gp_arc04227->SetName("Add average daily evap");
gp_arc04227->SetInternal(false);
gp_arc04227->SetExpr(" AustinModel_determ.Decision_Units.DU_Evap * 3.401568");
gp_arc04227->SetTableID("");
gp_arc04227->SetTargetID("node03613");
gp_node03644->AddFlow(gp_arc04227);

gp_arc04360=(Flow*)TestModelBm->GenModelObject(OME_FLOW);
gp_arc04360->SetID("arc04360");
gp_arc04360->SetName("Montly Sum");
gp_arc04360->SetInternal(false);
gp_arc04360->SetExpr("(if sum([AustinModel_determ.montly_averages.retrieve_sum])==1.0 then  AustinModel_determ.Decision_Units.Monthly_Evap  else 0.0 end)");
gp_arc04360->SetTableID("");
gp_arc04360->SetSourceID("node03613");
gp_node03644->AddFlow(gp_arc04360);

gp_node00017=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node00017->SetID("node00017");
gp_node00017->SetName("Heat Wave Characteristics");
gp_node00017->SetInternal(false);
gp_node00017->SetInitNumInstances(1);
gp_node00017->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node00021=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node00021->SetID("node00021");
gp_node00021->SetName("Heat wave\ntemperatures");
gp_node00021->SetInternal(false);
gp_node00021->SetExpr("(if AustinModel_determ.Decision_Units.Average_air_temperatures > AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.Regional_Threshold_T  then 1.0 else 0.0 end)");
gp_node00021->SetTableID("");
gp_node00021->SetExpectsFile(false);
gp_node00021->SetInitOnly(false);
gp_node00017->AddVariable(gp_node00021);

gp_node00034=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node00034->SetID("node00034");
gp_node00034->SetName(" Heat effect");
gp_node00034->SetInternal(false);
gp_node00034->SetExpr("max(0.0, AustinModel_determ.Decision_Units.Average_air_temperatures - AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.Regional_Threshold_T )/ 5.2");
gp_node00034->SetTableID("");
gp_node00034->SetExpectsFile(false);
gp_node00034->SetInitOnly(false);
gp_node00017->AddVariable(gp_node00034);

gp_node00036=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node00036->SetID("node00036");
gp_node00036->SetName("A and B HW");
gp_node00036->SetInternal(false);
gp_node00036->SetExpr("AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.heatwave*max( AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.Period ,2.0)+ AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.Count_Heatwave ");
gp_node00036->SetTableID("");
gp_node00036->SetExpectsFile(false);
gp_node00036->SetInitOnly(false);
gp_node00017->AddVariable(gp_node00036);

gp_node00045=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node00045->SetID("node00045");
gp_node00045->SetName("Regional\nThreshold T");
gp_node00045->SetInternal(false);
gp_node00045->SetExpr(" 28.28");
gp_node00045->SetTableID("");
gp_node00045->SetAskVal(true);
gp_node00045->SetMinValue(25);
gp_node00045->SetMaxValue(35);
gp_node00045->SetExpectsFile(false);
gp_node00045->SetInitOnly(true);
gp_node00017->AddVariable(gp_node00045);

gp_node00046=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node00046->SetID("node00046");
gp_node00046->SetName("Intensity");
gp_node00046->SetInternal(false);
gp_node00046->SetExpr("(if AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.Period ==0.0 then 0.0 else AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.cumulative_temperatures/ AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.Period  end)");
gp_node00046->SetTableID("");
gp_node00046->SetExpectsFile(false);
gp_node00046->SetInitOnly(false);
gp_node00017->AddVariable(gp_node00046);

gp_node00047=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node00047->SetID("node00047");
gp_node00047->SetName("Heat wave starts");
gp_node00047->SetInternal(false);
gp_node00047->SetExpr("(if last( AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.Heat_wave_temperatures )==0.0 and  AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.Heat_wave_temperatures ==1.0 then  AustinModel_determ.JDAY -121.0 else 366.0 end)");
gp_node00047->SetTableID("");
gp_node00047->SetExpectsFile(false);
gp_node00047->SetInitOnly(false);
gp_node00017->AddVariable(gp_node00047);

gp_node00048=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node00048->SetID("node00048");
gp_node00048->SetName("heatwave");
gp_node00048->SetInternal(false);
gp_node00048->SetExpr("(if AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.Period >=2.0 then 1.0 else 0.0 end)");
gp_node00048->SetTableID("");
gp_node00048->SetExpectsFile(false);
gp_node00048->SetInitOnly(false);
gp_node00017->AddVariable(gp_node00048);

gp_node00024=(StateVar*)TestModelBm->GenModelObject(OME_STATEVAR);
gp_node00024->SetID("node00024");
gp_node00024->SetName("cumulative\ntemperatures");
gp_node00024->SetInternal(false);
gp_node00024->SetExpr("0.0");
gp_node00024->SetTableID("");
gp_node00024->SetInitCondition(0);
gp_node00017->AddStateVar(gp_node00024);

gp_node00030=(StateVar*)TestModelBm->GenModelObject(OME_STATEVAR);
gp_node00030->SetID("node00030");
gp_node00030->SetName("Period");
gp_node00030->SetInternal(false);
gp_node00030->SetExpr("0.0");
gp_node00030->SetTableID("");
gp_node00030->SetInitCondition(0);
gp_node00017->AddStateVar(gp_node00030);

gp_node00052=(StateVar*)TestModelBm->GenModelObject(OME_STATEVAR);
gp_node00052->SetID("node00052");
gp_node00052->SetName("Count Heatwave");
gp_node00052->SetInternal(false);
gp_node00052->SetExpr("1.0");
gp_node00052->SetTableID("");
gp_node00052->SetInitCondition(0);
gp_node00017->AddStateVar(gp_node00052);

gp_arc01510=(Flow*)TestModelBm->GenModelObject(OME_FLOW);
gp_arc01510->SetID("arc01510");
gp_arc01510->SetName("add time");
gp_arc01510->SetInternal(false);
gp_arc01510->SetExpr(" AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.Heat_wave_temperatures ");
gp_arc01510->SetTableID("");
gp_arc01510->SetTargetID("node00030");
gp_node00017->AddFlow(gp_arc01510);

gp_arc01203=(Flow*)TestModelBm->GenModelObject(OME_FLOW);
gp_arc01203->SetID("arc01203");
gp_arc01203->SetName("Add High\ntempreatures");
gp_arc01203->SetInternal(false);
gp_arc01203->SetExpr(" AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.Heat_wave_temperatures *max(0.0, AustinModel_determ.Decision_Units.Average_air_temperatures - AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.Regional_Threshold_T )");
gp_arc01203->SetTableID("");
gp_arc01203->SetTargetID("node00024");
gp_node00017->AddFlow(gp_arc01203);

gp_arc01406=(Flow*)TestModelBm->GenModelObject(OME_FLOW);
gp_arc01406->SetID("arc01406");
gp_arc01406->SetName("reset intensity");
gp_arc01406->SetInternal(false);
gp_arc01406->SetExpr("(1.0- AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.Heat_wave_temperatures )*AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.cumulative_temperatures");
gp_arc01406->SetTableID("");
gp_arc01406->SetSourceID("node00024");
gp_node00017->AddFlow(gp_arc01406);

gp_arc01511=(Flow*)TestModelBm->GenModelObject(OME_FLOW);
gp_arc01511->SetID("arc01511");
gp_arc01511->SetName("end of period");
gp_arc01511->SetInternal(false);
gp_arc01511->SetExpr("(1.0- AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.Heat_wave_temperatures )* AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.Period ");
gp_arc01511->SetTableID("");
gp_arc01511->SetSourceID("node00030");
gp_node00017->AddFlow(gp_arc01511);

gp_arc01738=(Flow*)TestModelBm->GenModelObject(OME_FLOW);
gp_arc01738->SetID("arc01738");
gp_arc01738->SetName("Next wave");
gp_arc01738->SetInternal(false);
gp_arc01738->SetExpr("(if AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.end_of_period>0.0 then 1.0 else 0.0 end)");
gp_arc01738->SetTableID("");
gp_arc01738->SetTargetID("node00052");
gp_node00017->AddFlow(gp_arc01738);

gp_arc01739=(Flow*)TestModelBm->GenModelObject(OME_FLOW);
gp_arc01739->SetID("arc01739");
gp_arc01739->SetName("reset to 0");
gp_arc01739->SetInternal(false);
gp_arc01739->SetExpr("(if AustinModel_determ.JDAY ==273.0 then  AustinModel_determ.Decision_Units.Heat_Wave_Characteristics.Count_Heatwave -1.0 else 0.0 end)");
gp_arc01739->SetTableID("");
gp_arc01739->SetSourceID("node00052");
gp_node00017->AddFlow(gp_arc01739);

gp_node03644->AddSubmodel(gp_node00017);

gp_node03967=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03967->SetID("node03967");
gp_node03967->SetName("Social Fabric");
gp_node03967->SetInternal(false);
gp_node03967->SetInitNumInstances(1);
gp_node03967->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node04013=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04013->SetID("node04013");
gp_node04013->SetName("Rural ratio ");
gp_node04013->SetInternal(false);
gp_node04013->SetExpr("");
gp_node04013->SetTableID("");
gp_node04013->SetExpectsFile(true);
gp_node04013->SetInitOnly(true);
gp_node03967->AddVariable(gp_node04013);

gp_node04015=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04015->SetID("node04015");
gp_node04015->SetName("Employed\nratio");
gp_node04015->SetInternal(false);
gp_node04015->SetExpr("");
gp_node04015->SetTableID("");
gp_node04015->SetExpectsFile(true);
gp_node04015->SetInitOnly(true);
gp_node03967->AddVariable(gp_node04015);

gp_node04023=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04023->SetID("node04023");
gp_node04023->SetName("Wealth");
gp_node04023->SetInternal(false);
gp_node04023->SetExpr("");
gp_node04023->SetTableID("");
gp_node04023->SetExpectsFile(true);
gp_node04023->SetInitOnly(true);
gp_node03967->AddVariable(gp_node04023);

gp_node04024=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04024->SetID("node04024");
gp_node04024->SetName("Wealth\nposition");
gp_node04024->SetInternal(false);
gp_node04024->SetExpr("firsttrue( AustinModel_determ.Decision_Units.Social_Fabric.Wealth <[ AustinModel_determ.Wealth_Upper_boundaries ])");
gp_node04024->SetTableID("");
gp_node04024->SetExpectsFile(false);
gp_node04024->SetInitOnly(false);
gp_node03967->AddVariable(gp_node04024);

gp_node04019=(VarArray*)TestModelBm->GenModelObject(OME_VARARRAY);
gp_node04019->SetID("node04019");
gp_node04019->SetName("Race Eth\nGen Age");
gp_node04019->SetInternal(false);
gp_node04019->SetExpr("makearray(1.0,224.0)");
gp_node04019->SetTableID("");
gp_node04019->SetAsInt(true);
gp_node04019->SetExpectsFile(false);
gp_node04019->SetInitOnly(false);
IndVec node04019_dims=IndVec(0);
gp_node04019->OverrideDims(node04019_dims);
gp_node03967->AddVariable(gp_node04019);

gp_node04020=(VarArray*)TestModelBm->GenModelObject(OME_VARARRAY);
gp_node04020->SetID("node04020");
gp_node04020->SetName("Life\nStyle");
gp_node04020->SetInternal(false);
gp_node04020->SetExpr("[1.0- AustinModel_determ.Decision_Units.Social_Fabric.Rural_ratio_ , AustinModel_determ.Decision_Units.Social_Fabric.Rural_ratio_ ]");
gp_node04020->SetTableID("");
gp_node04020->SetExpectsFile(true);
gp_node04020->SetInitOnly(false);
IndVec node04020_dims=IndVec(1);
node04020_dims[0]=2;
gp_node04020->OverrideDims(node04020_dims);
gp_node03967->AddVariable(gp_node04020);

gp_node04021=(VarArray*)TestModelBm->GenModelObject(OME_VARARRAY);
gp_node04021->SetID("node04021");
gp_node04021->SetName("Employment");
gp_node04021->SetInternal(false);
gp_node04021->SetExpr("[ AustinModel_determ.Decision_Units.Social_Fabric.Employed_ratio ,1.0- AustinModel_determ.Decision_Units.Social_Fabric.Employed_ratio ]");
gp_node04021->SetTableID("");
gp_node04021->SetExpectsFile(false);
gp_node04021->SetInitOnly(false);
IndVec node04021_dims=IndVec(0);
gp_node04021->OverrideDims(node04021_dims);
gp_node03967->AddVariable(gp_node04021);

gp_node03644->AddSubmodel(gp_node03967);

gp_node03988=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03988->SetID("node03988");
gp_node03988->SetName("Land Covers");
gp_node03988->SetInternal(false);
gp_node03988->SetInitNumInstances(15);
gp_node03988->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node04030=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04030->SetID("node04030");
gp_node04030->SetName("Tree\ncover area");
gp_node04030->SetInternal(false);
gp_node04030->SetExpr(" AustinModel_determ.Decision_Units.Land_Covers.LandCover_Proportions * AustinModel_determ.Decision_Units.Area *element([ AustinModel_determ.Land_Cover_Attributes.Tree_cover ],index(1.0))");
gp_node04030->SetTableID("");
gp_node04030->SetExpectsFile(false);
gp_node04030->SetInitOnly(false);
gp_node03988->AddVariable(gp_node04030);

gp_node04034=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04034->SetID("node04034");
gp_node04034->SetName("Effect on wind");
gp_node04034->SetInternal(false);
gp_node04034->SetExpr(" AustinModel_determ.Decision_Units.Land_Covers.LandCover_Proportions *element([ AustinModel_determ.Land_Cover_Attributes.Effect_on_wind ],index(1.0))");
gp_node04034->SetTableID("");
gp_node04034->SetExpectsFile(false);
gp_node04034->SetInitOnly(false);
gp_node03988->AddVariable(gp_node04034);

gp_node04039=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04039->SetID("node04039");
gp_node04039->SetName("NDVI");
gp_node04039->SetInternal(false);
gp_node04039->SetExpr("element([ AustinModel_determ.Land_Cover_Attributes.NDVI ],index(1.0))");
gp_node04039->SetTableID("");
gp_node04039->SetExpectsFile(false);
gp_node04039->SetInitOnly(false);
gp_node03988->AddVariable(gp_node04039);

gp_node04041=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04041->SetID("node04041");
gp_node04041->SetName("weighted\nNDVI");
gp_node04041->SetInternal(false);
gp_node04041->SetExpr(" AustinModel_determ.Decision_Units.Land_Covers.NDVI * AustinModel_determ.Decision_Units.Land_Covers.LandCover_Proportions ");
gp_node04041->SetTableID("");
gp_node04041->SetExpectsFile(false);
gp_node04041->SetInitOnly(false);
gp_node03988->AddVariable(gp_node04041);

gp_node04046=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04046->SetID("node04046");
gp_node04046->SetName("Air temp");
gp_node04046->SetInternal(false);
gp_node04046->SetExpr("element([ AustinModel_determ.Land_Cover_Attributes.Air_T_intercept ],index(1.0))+element([ AustinModel_determ.Land_Cover_Attributes.Air_T_slope ],index(1.0))* AustinModel_determ.Decision_Units.Land_Covers.Canopy_temp ");
gp_node04046->SetTableID("");
gp_node04046->SetExpectsFile(false);
gp_node04046->SetInitOnly(false);
gp_node03988->AddVariable(gp_node04046);

gp_node04052=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04052->SetID("node04052");
gp_node04052->SetName("temperature\nreduction");
gp_node04052->SetInternal(false);
gp_node04052->SetExpr("AustinModel_determ.temp_reduction_per_unit_of_NDVI*element([ AustinModel_determ.Land_Cover_Attributes.NDVI ],index(1.0))+element([ AustinModel_determ.Land_Cover_Attributes.Temperature_buffer_capacity ],index(1.0))");
gp_node04052->SetTableID("");
gp_node04052->SetExpectsFile(false);
gp_node04052->SetInitOnly(false);
gp_node03988->AddVariable(gp_node04052);

gp_node04054=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04054->SetID("node04054");
gp_node04054->SetName("Canopy temp");
gp_node04054->SetInternal(false);
gp_node04054->SetExpr(" AustinModel_determ.Bare_Soil_Temperature -AustinModel_determ.Decision_Units.Land_Covers.temperature_reduction");
gp_node04054->SetTableID("");
gp_node04054->SetExpectsFile(false);
gp_node04054->SetInitOnly(false);
gp_node03988->AddVariable(gp_node04054);

gp_node04056=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04056->SetID("node04056");
gp_node04056->SetName("evaporation");
gp_node04056->SetInternal(false);
gp_node04056->SetExpr("(element([ AustinModel_determ.Land_Cover_Attributes.Net_radiant_Energy ],index(1.0))-AustinModel_determ.Decision_Units.Land_Covers.sensible_heat_flux- AustinModel_determ.Decision_Units.Land_Covers.Heat_storage )/2257.0");
gp_node04056->SetTableID("");
gp_node04056->SetExpectsFile(false);
gp_node04056->SetInitOnly(false);
gp_node03988->AddVariable(gp_node04056);

gp_node04060=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04060->SetID("node04060");
gp_node04060->SetName("LandCover\nProportions");
gp_node04060->SetInternal(false);
gp_node04060->SetExpr("");
gp_node04060->SetTableID("");
gp_node04060->SetExpectsFile(true);
gp_node04060->SetInitOnly(true);
gp_node03988->AddVariable(gp_node04060);

gp_node04064=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04064->SetID("node04064");
gp_node04064->SetName("sensible\nheat flux");
gp_node04064->SetInternal(false);
gp_node04064->SetExpr("element([ AustinModel_determ.Land_Cover_Attributes.Boundary_Layer_Heat_Conductance ],index(1.0))* 29.17 *( AustinModel_determ.Decision_Units.Land_Covers.Canopy_temp - AustinModel_determ.Air_T )");
gp_node04064->SetTableID("");
gp_node04064->SetExpectsFile(false);
gp_node04064->SetInitOnly(false);
gp_node03988->AddVariable(gp_node04064);

gp_node04043=(StateVar*)TestModelBm->GenModelObject(OME_STATEVAR);
gp_node04043->SetID("node04043");
gp_node04043->SetName("Heat storage");
gp_node04043->SetInternal(false);
gp_node04043->SetExpr("0.0");
gp_node04043->SetTableID("");
gp_node04043->SetInitCondition(0);
gp_node03988->AddStateVar(gp_node04043);

gp_node03644->AddSubmodel(gp_node03988);

gp_node04002=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node04002->SetID("node04002");
gp_node04002->SetName("Air pollution");
gp_node04002->SetInternal(false);
gp_node04002->SetInitNumInstances(6);
gp_node04002->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node04071=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04071->SetID("node04071");
gp_node04071->SetName("diffusion rt");
gp_node04071->SetInternal(false);
gp_node04071->SetExpr("(if element([ AustinModel_determ.Pollutant_attributes.Wind_sensitive ],index(1.0))==1.0 then (1.0- AustinModel_determ.Decision_Units.Air_stagnation )*element([AustinModel_determ.Pollutant_attributes.diffusion_rt],index(1.0)) else element([AustinModel_determ.Pollutant_attributes.diffusion_rt],index(1.0)) end)");
gp_node04071->SetTableID("");
gp_node04071->SetExpectsFile(false);
gp_node04071->SetInitOnly(false);
gp_node04002->AddVariable(gp_node04071);

gp_node04080=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04080->SetID("node04080");
gp_node04080->SetName("Pollutant\nremoval\npotential");
gp_node04080->SetInternal(false);
gp_node04080->SetExpr("sum([ AustinModel_determ.Decision_Units.Land_Covers.Tree_cover_area ])*element([AustinModel_determ.Pollutant_attributes.__pollutant_removal_rate],index(1.0))");
gp_node04080->SetTableID("");
gp_node04080->SetExpectsFile(false);
gp_node04080->SetInitOnly(false);
gp_node04002->AddVariable(gp_node04080);

gp_node04081=(StateVar*)TestModelBm->GenModelObject(OME_STATEVAR);
gp_node04081->SetID("node04081");
gp_node04081->SetName("Pollution\nlevel");
gp_node04081->SetInternal(false);
gp_node04081->SetExpr("0.0");
gp_node04081->SetTableID("");
gp_node04081->SetInitCondition(0);
gp_node04002->AddStateVar(gp_node04081);

gp_arc04201=(Flow*)TestModelBm->GenModelObject(OME_FLOW);
gp_arc04201->SetID("arc04201");
gp_arc04201->SetName("Pollutant\nproduction");
gp_arc04201->SetInternal(false);
gp_arc04201->SetExpr("(element([ AustinModel_determ.Pollutant_attributes.Global_concentration ],index(1.0))- AustinModel_determ.Decision_Units.Air_pollution.Pollution_level )*AustinModel_determ.Decision_Units.Air_pollution.diffusion_rt");
gp_arc04201->SetTableID("");
gp_arc04201->SetTargetID("node04081");
gp_node04002->AddFlow(gp_arc04201);

gp_arc04194=(Flow*)TestModelBm->GenModelObject(OME_FLOW);
gp_arc04194->SetID("arc04194");
gp_arc04194->SetName("Pollutant\nremoval");
gp_arc04194->SetInternal(false);
gp_arc04194->SetExpr("max(0.0,min( AustinModel_determ.Decision_Units.Air_pollution.Pollution_level , AustinModel_determ.Decision_Units.Air_pollution.Pollutant_removal_potential ))");
gp_arc04194->SetTableID("");
gp_arc04194->SetSourceID("node04081");
gp_node04002->AddFlow(gp_arc04194);

gp_node03644->AddSubmodel(gp_node04002);

gp_root->AddSubmodel(gp_node03644);

gp_node03655=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03655->SetID("node03655");
gp_node03655->SetName("NDVI years");
gp_node03655->SetInternal(false);
gp_node03655->SetInitNumInstances(8);
gp_node03655->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node04090=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node04090->SetID("node04090");
gp_node04090->SetName("biweekly period");
gp_node04090->SetInternal(false);
gp_node04090->SetInitNumInstances(27);
gp_node04090->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node04091=(VarArray*)TestModelBm->GenModelObject(OME_VARARRAY);
gp_node04091->SetID("node04091");
gp_node04091->SetName("NDVI data");
gp_node04091->SetInternal(false);
gp_node04091->SetExpr("makearray(1.0,9.0)");
gp_node04091->SetTableID("");
gp_node04091->SetAskVal(true);
gp_node04091->SetMinValue(0);
gp_node04091->SetMaxValue(1);
gp_node04091->SetExpectsFile(false);
gp_node04091->SetInitOnly(false);
IndVec node04091_dims=IndVec(0);
gp_node04091->OverrideDims(node04091_dims);
gp_node04090->AddVariable(gp_node04091);

gp_node03655->AddSubmodel(gp_node04090);

gp_root->AddSubmodel(gp_node03655);

gp_node03700=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03700->SetID("node03700");
gp_node03700->SetName("GCC input");
gp_node03700->SetInternal(false);
gp_node03700->SetInitNumInstances(1);
gp_node03700->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node04093=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04093->SetID("node04093");
gp_node04093->SetName("GCC M Temp");
gp_node04093->SetInternal(false);
gp_node04093->SetExpr("element(element([[ AustinModel_determ.Load_GCC_scenario_outputs.Years.Temp ]],AustinModel_determ.month), AustinModel_determ.GCC_input.Year )- 273.15");
gp_node04093->SetTableID("");
gp_node04093->SetAskVal(true);
gp_node04093->SetMinValue(-5);
gp_node04093->SetMaxValue(50);
gp_node04093->SetExpectsFile(false);
gp_node04093->SetInitOnly(false);
gp_node03700->AddVariable(gp_node04093);

gp_node04094=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04094->SetID("node04094");
gp_node04094->SetName("Precip_sc");
gp_node04094->SetInternal(false);
gp_node04094->SetExpr("element(element([[ AustinModel_determ.Load_GCC_scenario_outputs.Years.Precip ]],AustinModel_determ.month), AustinModel_determ.GCC_input.Year )");
gp_node04094->SetTableID("");
gp_node04094->SetAskVal(true);
gp_node04094->SetMinValue(0);
gp_node04094->SetMaxValue(10);
gp_node04094->SetExpectsFile(false);
gp_node04094->SetInitOnly(false);
gp_node03700->AddVariable(gp_node04094);

gp_node04108=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04108->SetID("node04108");
gp_node04108->SetName("Year");
gp_node04108->SetInternal(false);
gp_node04108->SetExpr(" AustinModel_determ.Year -27.0");
gp_node04108->SetTableID("");
gp_node04108->SetExpectsFile(false);
gp_node04108->SetInitOnly(false);
gp_node03700->AddVariable(gp_node04108);

gp_node04112=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04112->SetID("node04112");
gp_node04112->SetName("GCC M\nRelative Humidity");
gp_node04112->SetInternal(false);
gp_node04112->SetExpr("element(element([[ AustinModel_determ.Load_GCC_scenario_outputs.Years.Humidity ]],AustinModel_determ.month), AustinModel_determ.GCC_input.Year )");
gp_node04112->SetTableID("");
gp_node04112->SetAskVal(true);
gp_node04112->SetMinValue(0);
gp_node04112->SetMaxValue(100);
gp_node04112->SetExpectsFile(false);
gp_node04112->SetInitOnly(false);
gp_node03700->AddVariable(gp_node04112);

gp_node04113=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04113->SetID("node04113");
gp_node04113->SetName("GCC Wind\nspeeds");
gp_node04113->SetInternal(false);
gp_node04113->SetExpr("element(element([[ AustinModel_determ.Load_GCC_scenario_outputs.Years.Wind ]],AustinModel_determ.month), AustinModel_determ.GCC_input.Year )");
gp_node04113->SetTableID("");
gp_node04113->SetExpectsFile(false);
gp_node04113->SetInitOnly(false);
gp_node03700->AddVariable(gp_node04113);

gp_node04105=(Spawner*)TestModelBm->GenModelObject(OME_SPAWNER);
gp_node04105->SetID("node04105");
gp_node04105->SetName("Start of GCC");
gp_node04105->SetInternal(false);
gp_node04105->SetExpr("( Year >27.0)");
gp_node04105->SetTableID("");
gp_node04105->SetConditional(true);
gp_node03700->AddSpawner(gp_node04105);

gp_root->AddSubmodel(gp_node03700);

gp_node03711=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03711->SetID("node03711");
gp_node03711->SetName("NDVI observed");
gp_node03711->SetInternal(false);
gp_node03711->SetInitNumInstances(9);
gp_node03711->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node04115=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04115->SetID("node04115");
gp_node04115->SetName("NDVI predicted");
gp_node04115->SetInternal(false);
gp_node04115->SetExpr(" AustinModel_determ.NDVI_observed.Intercept + AustinModel_determ.NDVI_observed.YA_T_slope * AustinModel_determ.Averaged_RA_Temp + AustinModel_determ.NDVI_observed.T14_slope * AustinModel_determ.RA_Temp + AustinModel_determ.NDVI_observed.RH14_slope * AustinModel_determ.RA_Rel_Hum + AustinModel_determ.NDVI_observed.Temp_limit_coeff *AustinModel_determ.temp_limit_sizer* AustinModel_determ.NDVI_observed.Temp_limit ");
gp_node04115->SetTableID("");
gp_node04115->SetExpectsFile(false);
gp_node04115->SetInitOnly(false);
gp_node03711->AddVariable(gp_node04115);

gp_node04117=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04117->SetID("node04117");
gp_node04117->SetName("T14 slope");
gp_node04117->SetInternal(false);
gp_node04117->SetExpr("");
gp_node04117->SetTableID("");
gp_node04117->SetExpectsFile(true);
gp_node04117->SetInitOnly(true);
gp_node03711->AddVariable(gp_node04117);

gp_node04118=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04118->SetID("node04118");
gp_node04118->SetName("Intercept");
gp_node04118->SetInternal(false);
gp_node04118->SetExpr("");
gp_node04118->SetTableID("");
gp_node04118->SetExpectsFile(true);
gp_node04118->SetInitOnly(true);
gp_node03711->AddVariable(gp_node04118);

gp_node04119=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04119->SetID("node04119");
gp_node04119->SetName("Temp limit");
gp_node04119->SetInternal(false);
gp_node04119->SetExpr("min(0.0, AustinModel_determ.NDVI_observed.Opt_temp - AustinModel_determ.RA_Temp )");
gp_node04119->SetTableID("");
gp_node04119->SetExpectsFile(false);
gp_node04119->SetInitOnly(false);
gp_node03711->AddVariable(gp_node04119);

gp_node04121=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04121->SetID("node04121");
gp_node04121->SetName("RH14 slope");
gp_node04121->SetInternal(false);
gp_node04121->SetExpr("");
gp_node04121->SetTableID("");
gp_node04121->SetExpectsFile(true);
gp_node04121->SetInitOnly(true);
gp_node03711->AddVariable(gp_node04121);

gp_node04122=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04122->SetID("node04122");
gp_node04122->SetName("YA T slope");
gp_node04122->SetInternal(false);
gp_node04122->SetExpr("");
gp_node04122->SetTableID("");
gp_node04122->SetExpectsFile(true);
gp_node04122->SetInitOnly(true);
gp_node03711->AddVariable(gp_node04122);

gp_node04123=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04123->SetID("node04123");
gp_node04123->SetName("Opt_temp");
gp_node04123->SetInternal(false);
gp_node04123->SetExpr("");
gp_node04123->SetTableID("");
gp_node04123->SetExpectsFile(true);
gp_node04123->SetInitOnly(true);
gp_node03711->AddVariable(gp_node04123);

gp_node04124=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04124->SetID("node04124");
gp_node04124->SetName("Temp limit coeff");
gp_node04124->SetInternal(false);
gp_node04124->SetExpr("");
gp_node04124->SetTableID("");
gp_node04124->SetExpectsFile(true);
gp_node04124->SetInitOnly(true);
gp_node03711->AddVariable(gp_node04124);

gp_node04130=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04130->SetID("node04130");
gp_node04130->SetName("NDVI");
gp_node04130->SetInternal(false);
gp_node04130->SetExpr("(if AustinModel_determ.NDVI_data_or_predictions ==0.0 then element([ AustinModel_determ.NDVIs ],index(1.0)) else  AustinModel_determ.NDVI_observed.NDVI_predicted  end)");
gp_node04130->SetTableID("");
gp_node04130->SetExpectsFile(false);
gp_node04130->SetInitOnly(false);
gp_node03711->AddVariable(gp_node04130);

gp_root->AddSubmodel(gp_node03711);

gp_node03717=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03717->SetID("node03717");
gp_node03717->SetName("montly averages");
gp_node03717->SetInternal(false);
gp_node03717->SetInitNumInstances(1);
gp_node03717->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node04135=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04135->SetID("node04135");
gp_node04135->SetName("Measured\nValues");
gp_node04135->SetInternal(false);
gp_node04135->SetExpr("element(element([[ AustinModel_determ._39_years_of_Lakesurface_Evap.Per_month.Texas_Water_Development_Board ]], AustinModel_determ.Year ),AustinModel_determ.montly_averages.month)");
gp_node04135->SetTableID("");
gp_node04135->SetExpectsFile(false);
gp_node04135->SetInitOnly(false);
gp_node03717->AddVariable(gp_node04135);

gp_node04138=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04138->SetID("node04138");
gp_node04138->SetName("month");
gp_node04138->SetInternal(false);
gp_node04138->SetExpr("firsttrue( AustinModel_determ.JDAY ==[31.0,59.0,90.0,120.0,151.0,181.0,212.0,243.0,273.0,304.0,334.0,365.0])");
gp_node04138->SetTableID("");
gp_node04138->SetExpectsFile(false);
gp_node04138->SetInitOnly(false);
gp_node03717->AddVariable(gp_node04138);

gp_node04140=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04140->SetID("node04140");
gp_node04140->SetName("year");
gp_node04140->SetInternal(false);
gp_node04140->SetExpr(" AustinModel_determ.Year +1972.0");
gp_node04140->SetTableID("");
gp_node04140->SetExpectsFile(false);
gp_node04140->SetInitOnly(false);
gp_node03717->AddVariable(gp_node04140);

gp_node04147=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04147->SetID("node04147");
gp_node04147->SetName("Montly Evaporation");
gp_node04147->SetInternal(false);
gp_node04147->SetExpr("sum([ AustinModel_determ.Decision_Units.DU_Evap_contribution ])");
gp_node04147->SetTableID("");
gp_node04147->SetExpectsFile(false);
gp_node04147->SetInitOnly(false);
gp_node03717->AddVariable(gp_node04147);

gp_node04150=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04150->SetID("node04150");
gp_node04150->SetName("retrieve sum");
gp_node04150->SetInternal(false);
gp_node04150->SetExpr("1.0");
gp_node04150->SetTableID("");
gp_node04150->SetExpectsFile(false);
gp_node04150->SetInitOnly(true);
gp_node03717->AddVariable(gp_node04150);

gp_node04142=(VarArray*)TestModelBm->GenModelObject(OME_VARARRAY);
gp_node04142->SetID("node04142");
gp_node04142->SetName("Comparison");
gp_node04142->SetInternal(false);
gp_node04142->SetExpr("[ AustinModel_determ.montly_averages.Measured_Values , AustinModel_determ.montly_averages.Montly_Evaporation ,AustinModel_determ.montly_averages.year]");
gp_node04142->SetTableID("");
gp_node04142->SetExpectsFile(false);
gp_node04142->SetInitOnly(false);
IndVec node04142_dims=IndVec(1);
node04142_dims[0]=3;
gp_node04142->OverrideDims(node04142_dims);
gp_node03717->AddVariable(gp_node04142);

gp_node04144=(Spawner*)TestModelBm->GenModelObject(OME_SPAWNER);
gp_node04144->SetID("node04144");
gp_node04144->SetName("End of the month");
gp_node04144->SetInternal(false);
gp_node04144->SetExpr("( Year <=39.0 and any( JDAY ==[31.0,59.0,90.0,120.0,151.0,181.0,212.0,243.0,273.0,304.0,334.0,365.0]))");
gp_node04144->SetTableID("");
gp_node04144->SetConditional(true);
gp_node03717->AddSpawner(gp_node04144);

gp_root->AddSubmodel(gp_node03717);

gp_node03730=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03730->SetID("node03730");
gp_node03730->SetName("Calibration");
gp_node03730->SetInternal(false);
gp_node03730->SetInitNumInstances(1);
gp_node03730->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node04157=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04157->SetID("node04157");
gp_node04157->SetName("EMS model");
gp_node04157->SetInternal(false);
gp_node04157->SetExpr("sum([ AustinModel_determ.Decision_Units.EMS ])");
gp_node04157->SetTableID("");
gp_node04157->SetExpectsFile(false);
gp_node04157->SetInitOnly(false);
gp_node03730->AddVariable(gp_node04157);

gp_node04159=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04159->SetID("node04159");
gp_node04159->SetName("Complaints model");
gp_node04159->SetInternal(false);
gp_node04159->SetExpr("sum([ AustinModel_determ.Decision_Units.Complaints ])");
gp_node04159->SetTableID("");
gp_node04159->SetExpectsFile(false);
gp_node04159->SetInitOnly(false);
gp_node03730->AddVariable(gp_node04159);

gp_node04163=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04163->SetID("node04163");
gp_node04163->SetName("EMS data");
gp_node04163->SetInternal(false);
gp_node04163->SetExpr("element([ AustinModel_determ.Morbidity_data.EMS_data ], AustinModel_determ.JDAY -152.0)");
gp_node04163->SetTableID("");
gp_node04163->SetExpectsFile(false);
gp_node04163->SetInitOnly(false);
gp_node03730->AddVariable(gp_node04163);

gp_node04165=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04165->SetID("node04165");
gp_node04165->SetName("Complaints");
gp_node04165->SetInternal(false);
gp_node04165->SetExpr("element([ AustinModel_determ.Morbidity_data.Complaints_data ], AustinModel_determ.JDAY -152.0)");
gp_node04165->SetTableID("");
gp_node04165->SetExpectsFile(false);
gp_node04165->SetInitOnly(false);
gp_node03730->AddVariable(gp_node04165);

gp_node04169=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04169->SetID("node04169");
gp_node04169->SetName("EMS sq Err");
gp_node04169->SetInternal(false);
gp_node04169->SetExpr("( AustinModel_determ.Calibration.EMS_model - AustinModel_determ.Calibration.EMS_data )^2.0");
gp_node04169->SetTableID("");
gp_node04169->SetExpectsFile(false);
gp_node04169->SetInitOnly(false);
gp_node03730->AddVariable(gp_node04169);

gp_node04171=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04171->SetID("node04171");
gp_node04171->SetName("Complaints  sq err");
gp_node04171->SetInternal(false);
gp_node04171->SetExpr("( AustinModel_determ.Calibration.Complaints - AustinModel_determ.Calibration.Complaints_model )^2.0");
gp_node04171->SetTableID("");
gp_node04171->SetExpectsFile(false);
gp_node04171->SetInitOnly(false);
gp_node03730->AddVariable(gp_node04171);

gp_node04153=(Spawner*)TestModelBm->GenModelObject(OME_SPAWNER);
gp_node04153->SetID("node04153");
gp_node04153->SetName("cond1");
gp_node04153->SetInternal(false);
gp_node04153->SetExpr("( Year ==39.0 and  JDAY >152.0 and  JDAY <274.0)");
gp_node04153->SetTableID("");
gp_node04153->SetConditional(true);
gp_node03730->AddSpawner(gp_node04153);

gp_root->AddSubmodel(gp_node03730);

gp_node03731=(Model*)TestModelBm->GenModelObject(OME_MODEL);
gp_node03731->SetID("node03731");
gp_node03731->SetName("Morbidity data");
gp_node03731->SetInternal(false);
gp_node03731->SetInitNumInstances(121);
gp_node03731->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
gp_node04173=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04173->SetID("node04173");
gp_node04173->SetName("EMS data");
gp_node04173->SetInternal(false);
gp_node04173->SetExpr("");
gp_node04173->SetTableID("");
gp_node04173->SetExpectsFile(true);
gp_node04173->SetInitOnly(true);
gp_node03731->AddVariable(gp_node04173);

gp_node04174=(Variable*)TestModelBm->GenModelObject(OME_VARIABLE);
gp_node04174->SetID("node04174");
gp_node04174->SetName("Complaints data");
gp_node04174->SetInternal(false);
gp_node04174->SetExpr("");
gp_node04174->SetTableID("");
gp_node04174->SetExpectsFile(true);
gp_node04174->SetInitOnly(true);
gp_node03731->AddVariable(gp_node04174);

gp_root->AddSubmodel(gp_node03731);

gp_root->JoinLinks(true,true);
//Begin Lookup table


Register(gp_root);
Register(gp_node00059);
Register(gp_node00062);
Register(gp_node00228);
Register(gp_node03602);
Register(gp_node03603);
Register(gp_node03604);
Register(gp_node03605);
Register(gp_node03607);
Register(gp_node03612);
Register(gp_node03614);
Register(gp_node03617);
Register(gp_node03619);
Register(gp_node03620);
Register(gp_node03626);
Register(gp_node03628);
Register(gp_node03630);
Register(gp_node03632);
Register(gp_node03634);
Register(gp_node03638);
Register(gp_node03640);
Register(gp_node03651);
Register(gp_node03653);
Register(gp_node03668);
Register(gp_node03669);
Register(gp_node03671);
Register(gp_node03672);
Register(gp_node03680);
Register(gp_node03682);
Register(gp_node03683);
Register(gp_node03685);
Register(gp_node03693);
Register(gp_node03695);
Register(gp_node03697);
Register(gp_node03702);
Register(gp_node03704);
Register(gp_node03707);
Register(gp_node03709);
Register(gp_node03712);
Register(gp_node03718);
Register(gp_node03720);
Register(gp_node03723);
Register(gp_node03725);
Register(gp_node03594);
Register(gp_node03596);
Register(gp_node03598);
Register(gp_node03600);
Register(gp_node03641);
Register(gp_node03656);
Register(gp_node03657);
Register(gp_node03674);
Register(gp_node03677);
Register(gp_node03696);
Register(gp_node03699);
Register(gp_node03706);
Register(gp_node03728);
Register(gp_node00060);
Register(gp_node03637);
Register(gp_node03648);
Register(gp_node03694);
Register(gp_node03722);
Register(gp_node03622);
Register(gp_node03646);
Register(gp_node03663);
Register(gp_node03687);
Register(gp_arc03874);
Register(gp_arc04241);
Register(gp_arc04229);
Register(gp_arc04232);
Register(gp_arc04234);
Register(gp_arc04266);
Register(gp_arc04268);
Register(gp_node03593);
Register(gp_node03732);
Register(gp_node03734);
Register(gp_node03595);
Register(gp_node03739);
Register(gp_node03744);
Register(gp_node03746);
Register(gp_node03740);
Register(gp_node03749);
Register(gp_node03751);
Register(gp_node03741);
Register(gp_node03753);
Register(gp_node03755);
Register(gp_node03743);
Register(gp_node03756);
Register(gp_node03759);
Register(gp_node03606);
Register(gp_node03760);
Register(gp_node03765);
Register(gp_node03766);
Register(gp_node03767);
Register(gp_node03768);
Register(gp_node03608);
Register(gp_node03776);
Register(gp_node03781);
Register(gp_node03783);
Register(gp_node03784);
Register(gp_node03786);
Register(gp_node03611);
Register(gp_node03788);
Register(gp_node03790);
Register(gp_node03792);
Register(gp_node03794);
Register(gp_node03797);
Register(gp_node03800);
Register(gp_node03627);
Register(gp_node03804);
Register(gp_node03809);
Register(gp_node03812);
Register(gp_node03814);
Register(gp_node03817);
Register(gp_node03820);
Register(gp_node03821);
Register(gp_node03823);
Register(gp_node03973);
Register(gp_node03642);
Register(gp_node03825);
Register(gp_node03826);
Register(gp_node03828);
Register(gp_node03830);
Register(gp_node03834);
Register(gp_node03836);
Register(gp_node03841);
Register(gp_node03844);
Register(gp_node03848);
Register(gp_node03851);
Register(gp_node03853);
Register(gp_node03856);
Register(gp_node03862);
Register(gp_node03863);
Register(gp_node03869);
Register(gp_node03871);
Register(gp_node03872);
Register(gp_node03877);
Register(gp_node03880);
Register(gp_node03883);
Register(gp_node03893);
Register(gp_node03847);
Register(gp_node03904);
Register(gp_node03896);
Register(gp_arc03930);
Register(gp_node03644);
Register(gp_node00073);
Register(gp_node00074);
Register(gp_node00077);
Register(gp_node00079);
Register(gp_node00230);
Register(gp_node03927);
Register(gp_node03929);
Register(gp_node03934);
Register(gp_node03939);
Register(gp_node03950);
Register(gp_node03952);
Register(gp_node03954);
Register(gp_node03958);
Register(gp_node03963);
Register(gp_node03965);
Register(gp_node03970);
Register(gp_node03971);
Register(gp_node03975);
Register(gp_node03977);
Register(gp_node03979);
Register(gp_node03983);
Register(gp_node03990);
Register(gp_node04000);
Register(gp_node04004);
Register(gp_node03961);
Register(gp_node03613);
Register(gp_arc04227);
Register(gp_arc04360);
Register(gp_node00017);
Register(gp_node00021);
Register(gp_node00034);
Register(gp_node00036);
Register(gp_node00045);
Register(gp_node00046);
Register(gp_node00047);
Register(gp_node00048);
Register(gp_node00024);
Register(gp_node00030);
Register(gp_node00052);
Register(gp_arc01510);
Register(gp_arc01203);
Register(gp_arc01406);
Register(gp_arc01511);
Register(gp_arc01738);
Register(gp_arc01739);
Register(gp_node03967);
Register(gp_node04013);
Register(gp_node04015);
Register(gp_node04023);
Register(gp_node04024);
Register(gp_node04019);
Register(gp_node04020);
Register(gp_node04021);
Register(gp_node03988);
Register(gp_node04030);
Register(gp_node04034);
Register(gp_node04039);
Register(gp_node04041);
Register(gp_node04046);
Register(gp_node04052);
Register(gp_node04054);
Register(gp_node04056);
Register(gp_node04060);
Register(gp_node04064);
Register(gp_node04043);
Register(gp_node04002);
Register(gp_node04071);
Register(gp_node04080);
Register(gp_node04081);
Register(gp_arc04201);
Register(gp_arc04194);
Register(gp_node03655);
Register(gp_node04090);
Register(gp_node04091);
Register(gp_node03700);
Register(gp_node04093);
Register(gp_node04094);
Register(gp_node04108);
Register(gp_node04112);
Register(gp_node04113);
Register(gp_node04105);
Register(gp_node03711);
Register(gp_node04115);
Register(gp_node04117);
Register(gp_node04118);
Register(gp_node04119);
Register(gp_node04121);
Register(gp_node04122);
Register(gp_node04123);
Register(gp_node04124);
Register(gp_node04130);
Register(gp_node03717);
Register(gp_node04135);
Register(gp_node04138);
Register(gp_node04140);
Register(gp_node04147);
Register(gp_node04150);
Register(gp_node04142);
Register(gp_node04144);
Register(gp_node03730);
Register(gp_node04157);
Register(gp_node04159);
Register(gp_node04163);
Register(gp_node04165);
Register(gp_node04169);
Register(gp_node04171);
Register(gp_node04153);
Register(gp_node03731);
Register(gp_node04173);
Register(gp_node04174);
gp_root->JoinAliases();
return gp_root;
}

void TestModel::DoInit(const OME_SCALAR & TestModelTime, BaseManager* TestModelBm,ISpatialDataProvider* TestModelSDP)
{
using namespace TestModelDefs;
using namespace OMECFuncs;
Model& root=*gp_root;
Variable& node00059=*gp_node00059;
Variable& node00062=*gp_node00062;
Variable& node00228=*gp_node00228;
Variable& node03602=*gp_node03602;
Variable& node03603=*gp_node03603;
Variable& node03604=*gp_node03604;
Variable& node03605=*gp_node03605;
Variable& node03607=*gp_node03607;
Variable& node03612=*gp_node03612;
Variable& node03614=*gp_node03614;
Variable& node03617=*gp_node03617;
Variable& node03619=*gp_node03619;
Variable& node03620=*gp_node03620;
Variable& node03626=*gp_node03626;
Variable& node03628=*gp_node03628;
Variable& node03630=*gp_node03630;
Variable& node03632=*gp_node03632;
Variable& node03634=*gp_node03634;
Variable& node03638=*gp_node03638;
Variable& node03640=*gp_node03640;
Variable& node03651=*gp_node03651;
Variable& node03653=*gp_node03653;
Variable& node03668=*gp_node03668;
Variable& node03669=*gp_node03669;
Variable& node03671=*gp_node03671;
Variable& node03672=*gp_node03672;
Variable& node03680=*gp_node03680;
Variable& node03682=*gp_node03682;
Variable& node03683=*gp_node03683;
Variable& node03685=*gp_node03685;
Variable& node03693=*gp_node03693;
Variable& node03695=*gp_node03695;
Variable& node03697=*gp_node03697;
Variable& node03702=*gp_node03702;
Variable& node03704=*gp_node03704;
Variable& node03707=*gp_node03707;
Variable& node03709=*gp_node03709;
Variable& node03712=*gp_node03712;
Variable& node03718=*gp_node03718;
Variable& node03720=*gp_node03720;
Variable& node03723=*gp_node03723;
Variable& node03725=*gp_node03725;
VarArray& node03594=*gp_node03594;
VarArray& node03596=*gp_node03596;
VarArray& node03598=*gp_node03598;
VarArray& node03600=*gp_node03600;
VarArray& node03641=*gp_node03641;
VarArray& node03656=*gp_node03656;
VarArray& node03657=*gp_node03657;
VarArray& node03674=*gp_node03674;
VarArray& node03677=*gp_node03677;
VarArray& node03696=*gp_node03696;
VarArray& node03699=*gp_node03699;
VarArray& node03706=*gp_node03706;
VarArray& node03728=*gp_node03728;
TimeSeriesVar& node00060=*gp_node00060;
TimeSeriesVar& node03637=*gp_node03637;
TimeSeriesVar& node03648=*gp_node03648;
TimeSeriesVar& node03694=*gp_node03694;
TimeSeriesVar& node03722=*gp_node03722;
StateVar& node03622=*gp_node03622;
StateVar& node03646=*gp_node03646;
StateVar& node03663=*gp_node03663;
StateVar& node03687=*gp_node03687;
Flow& arc03874=*gp_arc03874;
Flow& arc04241=*gp_arc04241;
Flow& arc04229=*gp_arc04229;
Flow& arc04232=*gp_arc04232;
Flow& arc04234=*gp_arc04234;
Flow& arc04266=*gp_arc04266;
Flow& arc04268=*gp_arc04268;
Model& node03593=*gp_node03593;
Model& node03732=*gp_node03732;
Variable& node03734=*gp_node03734;
Model& node03595=*gp_node03595;
Model& node03739=*gp_node03739;
Model& node03744=*gp_node03744;
Variable& node03746=*gp_node03746;
Model& node03740=*gp_node03740;
Model& node03749=*gp_node03749;
Variable& node03751=*gp_node03751;
Model& node03741=*gp_node03741;
Model& node03753=*gp_node03753;
Variable& node03755=*gp_node03755;
Model& node03743=*gp_node03743;
Model& node03756=*gp_node03756;
Variable& node03759=*gp_node03759;
Model& node03606=*gp_node03606;
Model& node03760=*gp_node03760;
Variable& node03765=*gp_node03765;
Variable& node03766=*gp_node03766;
Variable& node03767=*gp_node03767;
Variable& node03768=*gp_node03768;
Model& node03608=*gp_node03608;
Model& node03776=*gp_node03776;
Variable& node03781=*gp_node03781;
Variable& node03783=*gp_node03783;
Variable& node03784=*gp_node03784;
Variable& node03786=*gp_node03786;
Model& node03611=*gp_node03611;
Variable& node03788=*gp_node03788;
Variable& node03790=*gp_node03790;
Variable& node03792=*gp_node03792;
Variable& node03794=*gp_node03794;
Variable& node03797=*gp_node03797;
Variable& node03800=*gp_node03800;
Model& node03627=*gp_node03627;
Variable& node03804=*gp_node03804;
Variable& node03809=*gp_node03809;
Variable& node03812=*gp_node03812;
Variable& node03814=*gp_node03814;
Variable& node03817=*gp_node03817;
Variable& node03820=*gp_node03820;
Variable& node03821=*gp_node03821;
Variable& node03823=*gp_node03823;
Variable& node03973=*gp_node03973;
Model& node03642=*gp_node03642;
Variable& node03825=*gp_node03825;
Variable& node03826=*gp_node03826;
Variable& node03828=*gp_node03828;
Variable& node03830=*gp_node03830;
Variable& node03834=*gp_node03834;
Variable& node03836=*gp_node03836;
Variable& node03841=*gp_node03841;
Variable& node03844=*gp_node03844;
Variable& node03848=*gp_node03848;
Variable& node03851=*gp_node03851;
Variable& node03853=*gp_node03853;
Variable& node03856=*gp_node03856;
Variable& node03862=*gp_node03862;
Variable& node03863=*gp_node03863;
Variable& node03869=*gp_node03869;
Variable& node03871=*gp_node03871;
Variable& node03872=*gp_node03872;
Variable& node03877=*gp_node03877;
Variable& node03880=*gp_node03880;
Variable& node03883=*gp_node03883;
Variable& node03893=*gp_node03893;
Model& node03847=*gp_node03847;
Variable& node03904=*gp_node03904;
StateVar& node03896=*gp_node03896;
Flow& arc03930=*gp_arc03930;
Model& node03644=*gp_node03644;
Variable& node00073=*gp_node00073;
Variable& node00074=*gp_node00074;
Variable& node00077=*gp_node00077;
Variable& node00079=*gp_node00079;
Variable& node00230=*gp_node00230;
Variable& node03927=*gp_node03927;
Variable& node03929=*gp_node03929;
Variable& node03934=*gp_node03934;
Variable& node03939=*gp_node03939;
Variable& node03950=*gp_node03950;
Variable& node03952=*gp_node03952;
Variable& node03954=*gp_node03954;
Variable& node03958=*gp_node03958;
Variable& node03963=*gp_node03963;
Variable& node03965=*gp_node03965;
Variable& node03970=*gp_node03970;
Variable& node03971=*gp_node03971;
Variable& node03975=*gp_node03975;
Variable& node03977=*gp_node03977;
Variable& node03979=*gp_node03979;
Variable& node03983=*gp_node03983;
Variable& node03990=*gp_node03990;
Variable& node04000=*gp_node04000;
Variable& node04004=*gp_node04004;
VarArray& node03961=*gp_node03961;
StateVar& node03613=*gp_node03613;
Flow& arc04227=*gp_arc04227;
Flow& arc04360=*gp_arc04360;
Model& node00017=*gp_node00017;
Variable& node00021=*gp_node00021;
Variable& node00034=*gp_node00034;
Variable& node00036=*gp_node00036;
Variable& node00045=*gp_node00045;
Variable& node00046=*gp_node00046;
Variable& node00047=*gp_node00047;
Variable& node00048=*gp_node00048;
StateVar& node00024=*gp_node00024;
StateVar& node00030=*gp_node00030;
StateVar& node00052=*gp_node00052;
Flow& arc01510=*gp_arc01510;
Flow& arc01203=*gp_arc01203;
Flow& arc01406=*gp_arc01406;
Flow& arc01511=*gp_arc01511;
Flow& arc01738=*gp_arc01738;
Flow& arc01739=*gp_arc01739;
Model& node03967=*gp_node03967;
Variable& node04013=*gp_node04013;
Variable& node04015=*gp_node04015;
Variable& node04023=*gp_node04023;
Variable& node04024=*gp_node04024;
VarArray& node04019=*gp_node04019;
VarArray& node04020=*gp_node04020;
VarArray& node04021=*gp_node04021;
Model& node03988=*gp_node03988;
Variable& node04030=*gp_node04030;
Variable& node04034=*gp_node04034;
Variable& node04039=*gp_node04039;
Variable& node04041=*gp_node04041;
Variable& node04046=*gp_node04046;
Variable& node04052=*gp_node04052;
Variable& node04054=*gp_node04054;
Variable& node04056=*gp_node04056;
Variable& node04060=*gp_node04060;
Variable& node04064=*gp_node04064;
StateVar& node04043=*gp_node04043;
Model& node04002=*gp_node04002;
Variable& node04071=*gp_node04071;
Variable& node04080=*gp_node04080;
StateVar& node04081=*gp_node04081;
Flow& arc04201=*gp_arc04201;
Flow& arc04194=*gp_arc04194;
Model& node03655=*gp_node03655;
Model& node04090=*gp_node04090;
VarArray& node04091=*gp_node04091;
Model& node03700=*gp_node03700;
Variable& node04093=*gp_node04093;
Variable& node04094=*gp_node04094;
Variable& node04108=*gp_node04108;
Variable& node04112=*gp_node04112;
Variable& node04113=*gp_node04113;
Spawner& node04105=*gp_node04105;
Model& node03711=*gp_node03711;
Variable& node04115=*gp_node04115;
Variable& node04117=*gp_node04117;
Variable& node04118=*gp_node04118;
Variable& node04119=*gp_node04119;
Variable& node04121=*gp_node04121;
Variable& node04122=*gp_node04122;
Variable& node04123=*gp_node04123;
Variable& node04124=*gp_node04124;
Variable& node04130=*gp_node04130;
Model& node03717=*gp_node03717;
Variable& node04135=*gp_node04135;
Variable& node04138=*gp_node04138;
Variable& node04140=*gp_node04140;
Variable& node04147=*gp_node04147;
Variable& node04150=*gp_node04150;
VarArray& node04142=*gp_node04142;
Spawner& node04144=*gp_node04144;
Model& node03730=*gp_node03730;
Variable& node04157=*gp_node04157;
Variable& node04159=*gp_node04159;
Variable& node04163=*gp_node04163;
Variable& node04165=*gp_node04165;
Variable& node04169=*gp_node04169;
Variable& node04171=*gp_node04171;
Spawner& node04153=*gp_node04153;
Model& node03731=*gp_node03731;
Variable& node04173=*gp_node04173;
Variable& node04174=*gp_node04174;

node03594.Allocate();
node03596.Allocate();
node03598.Allocate();
node03600.Allocate();
node03641.Allocate();
node03656.Allocate();
node03657.Allocate();
node03674.Allocate();
node03677.Allocate();
node03696.Allocate();
node03699.Allocate();
node03706.Allocate();
node03728.Allocate();
node03961.Allocate();
node04019.Allocate();
node04020.Allocate();
node04021.Allocate();
node04091.Allocate();
node04142.Allocate();
node03641.OverrideDims({696});
node03641.Allocate();
node03656.OverrideDims({12});
node03656.Allocate();
node03657.OverrideDims({12});
node03657.Allocate();
node03706.OverrideDims({12,11});
node03706.Allocate();
node04019.OverrideDims({224});
node04019.Allocate();
node04091.OverrideDims({9});
node04091.Allocate();
OME_SCALAR __OME__sum_37[5];
size_t __OME__sum_37_dims[]={5};
OME_SCALAR __OME__sum_7[5];
size_t __OME__sum_7_dims[]={5};
for(size_t __OME__iniLoop_0_i=0;__OME__iniLoop_0_i<5;++__OME__iniLoop_0_i)
{

__OME__sum_37[__OME__iniLoop_0_i]= 0.0;
__OME__sum_7[__OME__iniLoop_0_i]= 0.0;
}
OME_SCALAR __OME__sum_32[15];
size_t __OME__sum_32_dims[]={15};
OME_SCALAR __OME__sum_31[15];
size_t __OME__sum_31_dims[]={15};
OME_SCALAR __OME__sum_30[15];
size_t __OME__sum_30_dims[]={15};
//=-=-=-=-=-=-=-=-=-= node03607 (Air_T_reduction_by_water)
node03607.Reset(TestModelBm);
node03607=(OME_SCALAR)2.0;
node03607.BatchInitialize(TestModelTime,TestModelBm);

for(size_t node03731_i=0;node03731_i<121;++node03731_i)
{
node03731.SetCurrentInstance(node03731_i);

//=-=-=-=-=-=-=-=-=-= node04174 (Complaints_data)
node04174.Reset(TestModelBm);
node04174.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04173 (EMS_data)
node04173.Reset(TestModelBm);
node04173.BatchInitialize(TestModelTime,TestModelBm);

}
node03731.SetCurrentInstance(0);

for(size_t node03644_i=0;node03644_i<696;++node03644_i)
{
node03644.SetCurrentInstance(node03644_i);


//=-=-=-=-=-=-=-=-=-= node00024 (cumulative_temperatures)
node00024.Reset(TestModelBm);
node00024=(OME_SCALAR)0.0;
node00024.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node00030 (Period)
node00030.Reset(TestModelBm);
node00030=(OME_SCALAR)0.0;
node00030.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node00046 (Intensity)
node00046.Reset(TestModelBm);
node00046=(OME_SCALAR)(node00030==0.0 ? (OME_SCALAR)0.0 : (OME_SCALAR)node00024/node00030);
node00046.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node00048 (heatwave)
node00048.Reset(TestModelBm);
node00048=(OME_SCALAR)(node00030>=2.0 ? (OME_SCALAR)1.0 : (OME_SCALAR)0.0);
node00048.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node00045 (Regional_Threshold_T)
node00045.Reset(TestModelBm);
node00045=(OME_SCALAR)28.28;
node00045.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node00052 (Count_Heatwave)
node00052.Reset(TestModelBm);
node00052=(OME_SCALAR)1.0;
node00052.BatchInitialize(TestModelTime,TestModelBm);


//=-=-=-=-=-=-=-=-=-= node00073 (Cat_Mortality_rt)
node00073.Reset(TestModelBm);
node00073=(OME_SCALAR)node00048*(0.0374+((node00052>1.0 ? (OME_SCALAR)0.0504 : (OME_SCALAR)0.0265))+node00046/1.8*0.249+node00030*0.038);
node00073.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03970 (Area)
node03970.Reset(TestModelBm);
node03970.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03927 (population)
node03927.Reset(TestModelBm);
node03927.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03613 (Monthly_Evap)
node03613.Reset(TestModelBm);
node03613=(OME_SCALAR)0.0;
node03613.BatchInitialize(TestModelTime,TestModelBm);

}
node03644.SetCurrentInstance(0);

//=-=-=-=-=-=-=-=-=-= node03626 (Start_year)
node03626.Reset(TestModelBm);
node03626=(OME_SCALAR)2011.0;
node03626.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03622 (Year)
node03622.Reset(TestModelBm);
node03622=(OME_SCALAR)node03626-1973.0;
node03622.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03638 (JDAY)
node03638.Reset(TestModelBm);
node03638=(OME_SCALAR)::fmod((currenttime(TestModelBm)),(365.0))+1.0;
node03638.BatchInitialize(TestModelTime,TestModelBm);

for(size_t node03717_i=0;node03717_i<1;++node03717_i)
{
node03717.SetCurrentInstance(node03717_i);

OME_SCALAR __OME__any_0;
OME_SCALAR __OME__tempArray_0[]={31.0,59.0,90.0,120.0,151.0,181.0,212.0,243.0,273.0,304.0,334.0,365.0};
size_t __OME__tempArray_0_dims[]={12};
__OME__any_0=0;
for(size_t __OME__tmpLoop_0_i=0;__OME__tmpLoop_0_i<12;++__OME__tmpLoop_0_i)
{

__OME__any_0=__OME__any_0 || node03638==__OME__tempArray_0[__OME__tmpLoop_0_i];
}
//=-=-=-=-=-=-=-=-=-= node04144 (End_of_the_month)
node04144.BatchInitialize(TestModelTime,TestModelBm);
node04144.EvalWithVal(node03622<=39.0&&__OME__any_0);

node03717.GetCurrentInstanceRef()->ApplyMortality();
if (node03717.GetCurrentInstanceActive())
{
//=-=-=-=-=-=-=-=-=-= node04150 (retrieve_sum)
node04150.Reset(TestModelBm);
node04150=(OME_SCALAR)1.0;
node04150.BatchInitialize(TestModelTime,TestModelBm);

} //node03717
}
node03717.SetCurrentInstance(0);

OME_SCALAR __OME__sum_1;
__OME__sum_1= 0.0;
OME_SCALAR __OME__sum_0;
__OME__sum_0= 0.0;
for(size_t __OME__tmpLoop_1_i=0;__OME__tmpLoop_1_i<696;++__OME__tmpLoop_1_i)
{
node03642.SetCurrentInstance(__OME__tmpLoop_1_i);
node03644.SetCurrentInstance(__OME__tmpLoop_1_i);

for(size_t node03847_i=0;node03847_i<5;++node03847_i)
{
node03847.SetCurrentInstance(node03847_i);

//=-=-=-=-=-=-=-=-=-= node03896 (cumulative_morbidity)
node03896.Reset(TestModelBm);
node03896=(OME_SCALAR)0.0;
node03896.BatchInitialize(TestModelTime,TestModelBm);

}
node03847.SetCurrentInstance(0);


//=-=-=-=-=-=-=-=-=-= node04013 (Rural_ratio_)
node04013.Reset(TestModelBm);
node04013.BatchInitialize(TestModelTime,TestModelBm);

OME_SCALAR __OME__tempArray_8[]={1.0-node04013,node04013};
size_t __OME__tempArray_8_dims[]={2};
//=-=-=-=-=-=-=-=-=-= node04015 (Employed_ratio)
node04015.Reset(TestModelBm);
node04015.BatchInitialize(TestModelTime,TestModelBm);

OME_SCALAR __OME__tempArray_9[]={node04015,1.0-node04015};
size_t __OME__tempArray_9_dims[]={2};
for(size_t __OME__tmpLoop_10_i=0;__OME__tmpLoop_10_i<2;++__OME__tmpLoop_10_i)
{

//=-=-=-=-=-=-=-=-=-= node04020 (Life_Style)
(*root.GetValueArchive())[node04020.GetModelIndex()+__OME__tmpLoop_10_i+2*(__OME__tmpLoop_1_i)]=(OME_SCALAR)__OME__tempArray_8[__OME__tmpLoop_10_i];
node04020.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04021 (Employment)
node04021.OverrideDims({2});
node04021.Allocate();
(*root.GetValueArchive())[node04021.GetModelIndex()+__OME__tmpLoop_10_i+2*(__OME__tmpLoop_1_i)]=(OME_SCALAR)__OME__tempArray_9[__OME__tmpLoop_10_i];
node04021.BatchInitialize(TestModelTime,TestModelBm);

}
for(size_t makeArray_node040191_i=0;makeArray_node040191_i<224;++makeArray_node040191_i)
{

(*root.GetValueArchive())[node04019.GetModelIndex()+makeArray_node040191_i+224*(__OME__tmpLoop_1_i)]=(OME_SCALAR)1.0;

}
//=-=-=-=-=-=-=-=-=-= node04019 (Race_Eth_Gen_Age)
node04019.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04023 (Wealth)
node04023.Reset(TestModelBm);
node04023.BatchInitialize(TestModelTime,TestModelBm);


for(size_t node03988_i=0;node03988_i<15;++node03988_i)
{
node03988.SetCurrentInstance(node03988_i);

//=-=-=-=-=-=-=-=-=-= node04043 (Heat_storage)
node04043.Reset(TestModelBm);
node04043=(OME_SCALAR)0.0;
node04043.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04060 (LandCover_Proportions)
node04060.Reset(TestModelBm);
node04060.BatchInitialize(TestModelTime,TestModelBm);

}
node03988.SetCurrentInstance(0);

for(size_t node04002_i=0;node04002_i<6;++node04002_i)
{
node04002.SetCurrentInstance(node04002_i);

//=-=-=-=-=-=-=-=-=-= node04081 (Pollution_level)
node04081.Reset(TestModelBm);
node04081=(OME_SCALAR)0.0;
node04081.BatchInitialize(TestModelTime,TestModelBm);

}
node04002.SetCurrentInstance(0);

__OME__sum_0+=(*root.GetValueArchive())[node03970.GetModelIndex()+__OME__tmpLoop_1_i];
__OME__sum_1+=(*root.GetValueArchive())[node03927.GetModelIndex()+__OME__tmpLoop_1_i];
(*root.GetValueArchive())[node03641.GetModelIndex()+__OME__tmpLoop_1_i]=(OME_SCALAR)1.0;

}
node03642.SetCurrentInstance(0);
node03644.SetCurrentInstance(0);

OME_SCALAR __OME__tempArray_1[]={33.0,17.0,1.0,34.0,18.0,2.0,35.0,19.0,3.0,36.0,20.0,4.0,37.0,21.0,5.0,38.0,22.0,6.0,39.0,23.0,7.0,40.0,24.0,8.0,41.0,25.0,9.0,42.0,26.0,10.0,43.0,27.0,11.0,44.0,28.0,12.0,45.0,29.0,13.0,46.0,30.0,14.0,47.0,31.0,15.0,48.0,32.0,16.0};
size_t __OME__tempArray_1_dims[]={48};
for(size_t __OME__tmpLoop_3_i=0;__OME__tmpLoop_3_i<48;++__OME__tmpLoop_3_i)
{

//=-=-=-=-=-=-=-=-=-= node03596 (Children)
(*root.GetValueArchive())[node03596.GetModelIndex()+__OME__tmpLoop_3_i]=(OME_SCALAR)__OME__tempArray_1[__OME__tmpLoop_3_i];
node03596.BatchInitialize(TestModelTime,TestModelBm);

}
OME_SCALAR __OME__tempArray_2[]={33.0,49.0,65.0,81.0,97.0,17.0,113.0,129.0,145.0,161.0,177.0,193.0,209.0,1.0,34.0,50.0,66.0,82.0,98.0,18.0,114.0,130.0,146.0,162.0,178.0,194.0,210.0,2.0,35.0,51.0,67.0,83.0,99.0,19.0,115.0,131.0,147.0,163.0,179.0,195.0,211.0,3.0,36.0,52.0,68.0,84.0,100.0,20.0,116.0,132.0,148.0,164.0,180.0,196.0,212.0,4.0,37.0,53.0,69.0,85.0,101.0,21.0,117.0,133.0,149.0,165.0,181.0,197.0,213.0,5.0,38.0,54.0,70.0,86.0,102.0,22.0,118.0,134.0,150.0,166.0,182.0,198.0,214.0,6.0,41.0,57.0,73.0,89.0,105.0,25.0,121.0,137.0,153.0,169.0,185.0,201.0,217.0,9.0,42.0,58.0,74.0,90.0,106.0,26.0,122.0,138.0,154.0,170.0,186.0,202.0,218.0,10.0,43.0,59.0,75.0,91.0,107.0,27.0,123.0,139.0,155.0,171.0,187.0,203.0,219.0,11.0,44.0,60.0,76.0,92.0,108.0,28.0,124.0,140.0,156.0,172.0,188.0,204.0,220.0,12.0,45.0,61.0,77.0,93.0,109.0,29.0,125.0,141.0,157.0,173.0,189.0,205.0,221.0,13.0,46.0,62.0,78.0,94.0,110.0,30.0,126.0,142.0,158.0,174.0,190.0,206.0,222.0,14.0};
size_t __OME__tempArray_2_dims[]={168};
for(size_t __OME__tmpLoop_4_i=0;__OME__tmpLoop_4_i<168;++__OME__tmpLoop_4_i)
{

//=-=-=-=-=-=-=-=-=-= node03598 (None_White)
(*root.GetValueArchive())[node03598.GetModelIndex()+__OME__tmpLoop_4_i]=(OME_SCALAR)__OME__tempArray_2[__OME__tmpLoop_4_i];
node03598.BatchInitialize(TestModelTime,TestModelBm);

}
OME_SCALAR __OME__tempArray_3[]={145.0,161.0,177.0,193.0,209.0,146.0,162.0,178.0,194.0,210.0,147.0,163.0,179.0,195.0,211.0,148.0,164.0,180.0,196.0,212.0,149.0,165.0,181.0,197.0,213.0,150.0,166.0,182.0,198.0,214.0,151.0,167.0,183.0,199.0,215.0,152.0,168.0,184.0,200.0,216.0,153.0,169.0,185.0,201.0,217.0,154.0,170.0,186.0,202.0,218.0,155.0,171.0,187.0,203.0,219.0,156.0,172.0,188.0,204.0,220.0,157.0,173.0,189.0,205.0,221.0,158.0,174.0,190.0,206.0,222.0,159.0,175.0,191.0,207.0,223.0,160.0,176.0,192.0,208.0,224.0};
size_t __OME__tempArray_3_dims[]={80};
for(size_t __OME__tmpLoop_5_i=0;__OME__tmpLoop_5_i<80;++__OME__tmpLoop_5_i)
{

//=-=-=-=-=-=-=-=-=-= node03600 (Elderly)
(*root.GetValueArchive())[node03600.GetModelIndex()+__OME__tmpLoop_5_i]=(OME_SCALAR)__OME__tempArray_3[__OME__tmpLoop_5_i];
node03600.BatchInitialize(TestModelTime,TestModelBm);

}
OME_SCALAR __OME__firsttrue_0;
OME_SCALAR __OME__tempArray_4[]={31.0,59.0,90.0,120.0,151.0,181.0,212.0,243.0,273.0,304.0,334.0,365.0};
size_t __OME__tempArray_4_dims[]={12};
__OME__firsttrue_0=0;
for(size_t __OME__tmpLoop_6_i=0;__OME__tmpLoop_6_i<12;++__OME__tmpLoop_6_i)
{
node03606.SetCurrentInstance(__OME__tmpLoop_6_i);

for(size_t makeArray_node037062_i=0;makeArray_node037062_i<11;++makeArray_node037062_i)
{

(*root.GetValueArchive())[node03706.GetModelIndex()+makeArray_node037062_i+11*(__OME__tmpLoop_6_i)]=(OME_SCALAR)1.0;

}
for(size_t node03760_i=0;node03760_i<100;++node03760_i)
{
node03760.SetCurrentInstance(node03760_i);

//=-=-=-=-=-=-=-=-=-= node03768 (Wind)
node03768.Reset(TestModelBm);
node03768=(OME_SCALAR)0.0;
node03768.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03767 (Temp)
node03767.Reset(TestModelBm);
node03767.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03765 (Precip)
node03765.Reset(TestModelBm);
node03765.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03766 (Humidity)
node03766.Reset(TestModelBm);
node03766.BatchInitialize(TestModelTime,TestModelBm);

}
node03760.SetCurrentInstance(0);

if(!__OME__firsttrue_0 && node03638<=__OME__tempArray_4[__OME__tmpLoop_6_i]) __OME__firsttrue_0=__OME__tmpLoop_6_i+1;
(*root.GetValueArchive())[node03656.GetModelIndex()+__OME__tmpLoop_6_i]=(OME_SCALAR)1.0;

(*root.GetValueArchive())[node03657.GetModelIndex()+__OME__tmpLoop_6_i]=(OME_SCALAR)1.0;

}
node03606.SetCurrentInstance(0);

OME_SCALAR __OME__firsttrue_1;
OME_SCALAR __OME__tempArray_5[]={8.0,21.0,35.0,49.0,63.0,77.0,91.0,105.0,119.0,133.0,147.0,161.0,175.0,189.0,203.0,217.0,231.0,245.0,259.0,273.0,287.0,301.0,315.0,329.0,343.0,357.0,372.0};
size_t __OME__tempArray_5_dims[]={27};
__OME__firsttrue_1=0;
for(size_t __OME__tmpLoop_7_i=0;__OME__tmpLoop_7_i<27;++__OME__tmpLoop_7_i)
{

if(!__OME__firsttrue_1 && node03638<=__OME__tempArray_5[__OME__tmpLoop_7_i]) __OME__firsttrue_1=__OME__tmpLoop_7_i+1;
}
OME_SCALAR __OME__tempArray_6[]={10000.0,15000.0,20000.0,25000.0,30000.0,35000.0,40000.0,45000.0,50000.0,60000.0,70000.0,100000.0,125000.0,150000.0,200000.0,20000000.0};
size_t __OME__tempArray_6_dims[]={16};
for(size_t __OME__tmpLoop_8_i=0;__OME__tmpLoop_8_i<16;++__OME__tmpLoop_8_i)
{

//=-=-=-=-=-=-=-=-=-= node03674 (Wealth_Upper_boundaries)
(*root.GetValueArchive())[node03674.GetModelIndex()+__OME__tmpLoop_8_i]=(OME_SCALAR)__OME__tempArray_6[__OME__tmpLoop_8_i];
node03674.BatchInitialize(TestModelTime,TestModelBm);

}
OME_SCALAR __OME__firsttrue_2;
//=-=-=-=-=-=-=-=-=-= node03702 (Rain_Intensity)
node03702.Reset(TestModelBm);
node03702=(OME_SCALAR)10.0;
node03702.BatchInitialize(TestModelTime,TestModelBm);

OME_SCALAR __OME__tempArray_7[]={99.5,97.5,90.0,75.0,50.0,25.0,10.0,2.5,0.5,0.1,0.0};
size_t __OME__tempArray_7_dims[]={11};
__OME__firsttrue_2=0;
for(size_t __OME__tmpLoop_9_i=0;__OME__tmpLoop_9_i<11;++__OME__tmpLoop_9_i)
{

if(!__OME__firsttrue_2 && node03702>=__OME__tempArray_7[__OME__tmpLoop_9_i]) __OME__firsttrue_2=__OME__tmpLoop_9_i+1;
}
//=-=-=-=-=-=-=-=-=-= node03682 (averaging_time)
node03682.Reset(TestModelBm);
node03682=(OME_SCALAR)14.0;
node03682.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03646 (T)
node03646.Reset(TestModelBm);
node03646=(OME_SCALAR)node03682*9.7;
node03646.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03683 (RA_Temp)
node03683.Reset(TestModelBm);
node03683=(OME_SCALAR)node03646/node03682;
node03683.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03641 (Blgr_ids)
node03641.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03695 (Choice_of_Social_Fabric_Indicator)
node03695.Reset(TestModelBm);
node03695=(OME_SCALAR)2.0;
node03695.BatchInitialize(TestModelTime,TestModelBm);

for(size_t node03642_i=0;node03642_i<696;++node03642_i)
{
node03642.SetCurrentInstance(node03642_i);

OME_SCALAR __OME__sum_5[224];
size_t __OME__sum_5_dims[]={224};
for(size_t __OME__iniLoop_5_i=0;__OME__iniLoop_5_i<224;++__OME__iniLoop_5_i)
{

__OME__sum_5[__OME__iniLoop_5_i]= 0.0;
}
OME_SCALAR __OME__sum_2;
OME_SCALAR __OME__tmp_listGen_0[80]={};
__OME__tmp_listGen_0[0]=elementRaw(node04019,node03642_i+1,2)[node03600[0]-1];
__OME__tmp_listGen_0[1]=elementRaw(node04019,node03642_i+1,2)[node03600[1]-1];
__OME__tmp_listGen_0[2]=elementRaw(node04019,node03642_i+1,2)[node03600[2]-1];
__OME__tmp_listGen_0[3]=elementRaw(node04019,node03642_i+1,2)[node03600[3]-1];
__OME__tmp_listGen_0[4]=elementRaw(node04019,node03642_i+1,2)[node03600[4]-1];
__OME__tmp_listGen_0[5]=elementRaw(node04019,node03642_i+1,2)[node03600[5]-1];
__OME__tmp_listGen_0[6]=elementRaw(node04019,node03642_i+1,2)[node03600[6]-1];
__OME__tmp_listGen_0[7]=elementRaw(node04019,node03642_i+1,2)[node03600[7]-1];
__OME__tmp_listGen_0[8]=elementRaw(node04019,node03642_i+1,2)[node03600[8]-1];
__OME__tmp_listGen_0[9]=elementRaw(node04019,node03642_i+1,2)[node03600[9]-1];
__OME__tmp_listGen_0[10]=elementRaw(node04019,node03642_i+1,2)[node03600[10]-1];
__OME__tmp_listGen_0[11]=elementRaw(node04019,node03642_i+1,2)[node03600[11]-1];
__OME__tmp_listGen_0[12]=elementRaw(node04019,node03642_i+1,2)[node03600[12]-1];
__OME__tmp_listGen_0[13]=elementRaw(node04019,node03642_i+1,2)[node03600[13]-1];
__OME__tmp_listGen_0[14]=elementRaw(node04019,node03642_i+1,2)[node03600[14]-1];
__OME__tmp_listGen_0[15]=elementRaw(node04019,node03642_i+1,2)[node03600[15]-1];
__OME__tmp_listGen_0[16]=elementRaw(node04019,node03642_i+1,2)[node03600[16]-1];
__OME__tmp_listGen_0[17]=elementRaw(node04019,node03642_i+1,2)[node03600[17]-1];
__OME__tmp_listGen_0[18]=elementRaw(node04019,node03642_i+1,2)[node03600[18]-1];
__OME__tmp_listGen_0[19]=elementRaw(node04019,node03642_i+1,2)[node03600[19]-1];
__OME__tmp_listGen_0[20]=elementRaw(node04019,node03642_i+1,2)[node03600[20]-1];
__OME__tmp_listGen_0[21]=elementRaw(node04019,node03642_i+1,2)[node03600[21]-1];
__OME__tmp_listGen_0[22]=elementRaw(node04019,node03642_i+1,2)[node03600[22]-1];
__OME__tmp_listGen_0[23]=elementRaw(node04019,node03642_i+1,2)[node03600[23]-1];
__OME__tmp_listGen_0[24]=elementRaw(node04019,node03642_i+1,2)[node03600[24]-1];
__OME__tmp_listGen_0[25]=elementRaw(node04019,node03642_i+1,2)[node03600[25]-1];
__OME__tmp_listGen_0[26]=elementRaw(node04019,node03642_i+1,2)[node03600[26]-1];
__OME__tmp_listGen_0[27]=elementRaw(node04019,node03642_i+1,2)[node03600[27]-1];
__OME__tmp_listGen_0[28]=elementRaw(node04019,node03642_i+1,2)[node03600[28]-1];
__OME__tmp_listGen_0[29]=elementRaw(node04019,node03642_i+1,2)[node03600[29]-1];
__OME__tmp_listGen_0[30]=elementRaw(node04019,node03642_i+1,2)[node03600[30]-1];
__OME__tmp_listGen_0[31]=elementRaw(node04019,node03642_i+1,2)[node03600[31]-1];
__OME__tmp_listGen_0[32]=elementRaw(node04019,node03642_i+1,2)[node03600[32]-1];
__OME__tmp_listGen_0[33]=elementRaw(node04019,node03642_i+1,2)[node03600[33]-1];
__OME__tmp_listGen_0[34]=elementRaw(node04019,node03642_i+1,2)[node03600[34]-1];
__OME__tmp_listGen_0[35]=elementRaw(node04019,node03642_i+1,2)[node03600[35]-1];
__OME__tmp_listGen_0[36]=elementRaw(node04019,node03642_i+1,2)[node03600[36]-1];
__OME__tmp_listGen_0[37]=elementRaw(node04019,node03642_i+1,2)[node03600[37]-1];
__OME__tmp_listGen_0[38]=elementRaw(node04019,node03642_i+1,2)[node03600[38]-1];
__OME__tmp_listGen_0[39]=elementRaw(node04019,node03642_i+1,2)[node03600[39]-1];
__OME__tmp_listGen_0[40]=elementRaw(node04019,node03642_i+1,2)[node03600[40]-1];
__OME__tmp_listGen_0[41]=elementRaw(node04019,node03642_i+1,2)[node03600[41]-1];
__OME__tmp_listGen_0[42]=elementRaw(node04019,node03642_i+1,2)[node03600[42]-1];
__OME__tmp_listGen_0[43]=elementRaw(node04019,node03642_i+1,2)[node03600[43]-1];
__OME__tmp_listGen_0[44]=elementRaw(node04019,node03642_i+1,2)[node03600[44]-1];
__OME__tmp_listGen_0[45]=elementRaw(node04019,node03642_i+1,2)[node03600[45]-1];
__OME__tmp_listGen_0[46]=elementRaw(node04019,node03642_i+1,2)[node03600[46]-1];
__OME__tmp_listGen_0[47]=elementRaw(node04019,node03642_i+1,2)[node03600[47]-1];
__OME__tmp_listGen_0[48]=elementRaw(node04019,node03642_i+1,2)[node03600[48]-1];
__OME__tmp_listGen_0[49]=elementRaw(node04019,node03642_i+1,2)[node03600[49]-1];
__OME__tmp_listGen_0[50]=elementRaw(node04019,node03642_i+1,2)[node03600[50]-1];
__OME__tmp_listGen_0[51]=elementRaw(node04019,node03642_i+1,2)[node03600[51]-1];
__OME__tmp_listGen_0[52]=elementRaw(node04019,node03642_i+1,2)[node03600[52]-1];
__OME__tmp_listGen_0[53]=elementRaw(node04019,node03642_i+1,2)[node03600[53]-1];
__OME__tmp_listGen_0[54]=elementRaw(node04019,node03642_i+1,2)[node03600[54]-1];
__OME__tmp_listGen_0[55]=elementRaw(node04019,node03642_i+1,2)[node03600[55]-1];
__OME__tmp_listGen_0[56]=elementRaw(node04019,node03642_i+1,2)[node03600[56]-1];
__OME__tmp_listGen_0[57]=elementRaw(node04019,node03642_i+1,2)[node03600[57]-1];
__OME__tmp_listGen_0[58]=elementRaw(node04019,node03642_i+1,2)[node03600[58]-1];
__OME__tmp_listGen_0[59]=elementRaw(node04019,node03642_i+1,2)[node03600[59]-1];
__OME__tmp_listGen_0[60]=elementRaw(node04019,node03642_i+1,2)[node03600[60]-1];
__OME__tmp_listGen_0[61]=elementRaw(node04019,node03642_i+1,2)[node03600[61]-1];
__OME__tmp_listGen_0[62]=elementRaw(node04019,node03642_i+1,2)[node03600[62]-1];
__OME__tmp_listGen_0[63]=elementRaw(node04019,node03642_i+1,2)[node03600[63]-1];
__OME__tmp_listGen_0[64]=elementRaw(node04019,node03642_i+1,2)[node03600[64]-1];
__OME__tmp_listGen_0[65]=elementRaw(node04019,node03642_i+1,2)[node03600[65]-1];
__OME__tmp_listGen_0[66]=elementRaw(node04019,node03642_i+1,2)[node03600[66]-1];
__OME__tmp_listGen_0[67]=elementRaw(node04019,node03642_i+1,2)[node03600[67]-1];
__OME__tmp_listGen_0[68]=elementRaw(node04019,node03642_i+1,2)[node03600[68]-1];
__OME__tmp_listGen_0[69]=elementRaw(node04019,node03642_i+1,2)[node03600[69]-1];
__OME__tmp_listGen_0[70]=elementRaw(node04019,node03642_i+1,2)[node03600[70]-1];
__OME__tmp_listGen_0[71]=elementRaw(node04019,node03642_i+1,2)[node03600[71]-1];
__OME__tmp_listGen_0[72]=elementRaw(node04019,node03642_i+1,2)[node03600[72]-1];
__OME__tmp_listGen_0[73]=elementRaw(node04019,node03642_i+1,2)[node03600[73]-1];
__OME__tmp_listGen_0[74]=elementRaw(node04019,node03642_i+1,2)[node03600[74]-1];
__OME__tmp_listGen_0[75]=elementRaw(node04019,node03642_i+1,2)[node03600[75]-1];
__OME__tmp_listGen_0[76]=elementRaw(node04019,node03642_i+1,2)[node03600[76]-1];
__OME__tmp_listGen_0[77]=elementRaw(node04019,node03642_i+1,2)[node03600[77]-1];
__OME__tmp_listGen_0[78]=elementRaw(node04019,node03642_i+1,2)[node03600[78]-1];
__OME__tmp_listGen_0[79]=elementRaw(node04019,node03642_i+1,2)[node03600[79]-1];

__OME__sum_2= 0.0;
for(size_t __OME__tmpLoop_12_i=0;__OME__tmpLoop_12_i<80;++__OME__tmpLoop_12_i)
{

__OME__sum_2+=__OME__tmp_listGen_0[__OME__tmpLoop_12_i];
}
OME_SCALAR __OME__sum_3;
OME_SCALAR __OME__tmp_listGen_1[48]={};
__OME__tmp_listGen_1[0]=elementRaw(node04019,node03642_i+1,2)[node03596[0]-1];
__OME__tmp_listGen_1[1]=elementRaw(node04019,node03642_i+1,2)[node03596[1]-1];
__OME__tmp_listGen_1[2]=elementRaw(node04019,node03642_i+1,2)[node03596[2]-1];
__OME__tmp_listGen_1[3]=elementRaw(node04019,node03642_i+1,2)[node03596[3]-1];
__OME__tmp_listGen_1[4]=elementRaw(node04019,node03642_i+1,2)[node03596[4]-1];
__OME__tmp_listGen_1[5]=elementRaw(node04019,node03642_i+1,2)[node03596[5]-1];
__OME__tmp_listGen_1[6]=elementRaw(node04019,node03642_i+1,2)[node03596[6]-1];
__OME__tmp_listGen_1[7]=elementRaw(node04019,node03642_i+1,2)[node03596[7]-1];
__OME__tmp_listGen_1[8]=elementRaw(node04019,node03642_i+1,2)[node03596[8]-1];
__OME__tmp_listGen_1[9]=elementRaw(node04019,node03642_i+1,2)[node03596[9]-1];
__OME__tmp_listGen_1[10]=elementRaw(node04019,node03642_i+1,2)[node03596[10]-1];
__OME__tmp_listGen_1[11]=elementRaw(node04019,node03642_i+1,2)[node03596[11]-1];
__OME__tmp_listGen_1[12]=elementRaw(node04019,node03642_i+1,2)[node03596[12]-1];
__OME__tmp_listGen_1[13]=elementRaw(node04019,node03642_i+1,2)[node03596[13]-1];
__OME__tmp_listGen_1[14]=elementRaw(node04019,node03642_i+1,2)[node03596[14]-1];
__OME__tmp_listGen_1[15]=elementRaw(node04019,node03642_i+1,2)[node03596[15]-1];
__OME__tmp_listGen_1[16]=elementRaw(node04019,node03642_i+1,2)[node03596[16]-1];
__OME__tmp_listGen_1[17]=elementRaw(node04019,node03642_i+1,2)[node03596[17]-1];
__OME__tmp_listGen_1[18]=elementRaw(node04019,node03642_i+1,2)[node03596[18]-1];
__OME__tmp_listGen_1[19]=elementRaw(node04019,node03642_i+1,2)[node03596[19]-1];
__OME__tmp_listGen_1[20]=elementRaw(node04019,node03642_i+1,2)[node03596[20]-1];
__OME__tmp_listGen_1[21]=elementRaw(node04019,node03642_i+1,2)[node03596[21]-1];
__OME__tmp_listGen_1[22]=elementRaw(node04019,node03642_i+1,2)[node03596[22]-1];
__OME__tmp_listGen_1[23]=elementRaw(node04019,node03642_i+1,2)[node03596[23]-1];
__OME__tmp_listGen_1[24]=elementRaw(node04019,node03642_i+1,2)[node03596[24]-1];
__OME__tmp_listGen_1[25]=elementRaw(node04019,node03642_i+1,2)[node03596[25]-1];
__OME__tmp_listGen_1[26]=elementRaw(node04019,node03642_i+1,2)[node03596[26]-1];
__OME__tmp_listGen_1[27]=elementRaw(node04019,node03642_i+1,2)[node03596[27]-1];
__OME__tmp_listGen_1[28]=elementRaw(node04019,node03642_i+1,2)[node03596[28]-1];
__OME__tmp_listGen_1[29]=elementRaw(node04019,node03642_i+1,2)[node03596[29]-1];
__OME__tmp_listGen_1[30]=elementRaw(node04019,node03642_i+1,2)[node03596[30]-1];
__OME__tmp_listGen_1[31]=elementRaw(node04019,node03642_i+1,2)[node03596[31]-1];
__OME__tmp_listGen_1[32]=elementRaw(node04019,node03642_i+1,2)[node03596[32]-1];
__OME__tmp_listGen_1[33]=elementRaw(node04019,node03642_i+1,2)[node03596[33]-1];
__OME__tmp_listGen_1[34]=elementRaw(node04019,node03642_i+1,2)[node03596[34]-1];
__OME__tmp_listGen_1[35]=elementRaw(node04019,node03642_i+1,2)[node03596[35]-1];
__OME__tmp_listGen_1[36]=elementRaw(node04019,node03642_i+1,2)[node03596[36]-1];
__OME__tmp_listGen_1[37]=elementRaw(node04019,node03642_i+1,2)[node03596[37]-1];
__OME__tmp_listGen_1[38]=elementRaw(node04019,node03642_i+1,2)[node03596[38]-1];
__OME__tmp_listGen_1[39]=elementRaw(node04019,node03642_i+1,2)[node03596[39]-1];
__OME__tmp_listGen_1[40]=elementRaw(node04019,node03642_i+1,2)[node03596[40]-1];
__OME__tmp_listGen_1[41]=elementRaw(node04019,node03642_i+1,2)[node03596[41]-1];
__OME__tmp_listGen_1[42]=elementRaw(node04019,node03642_i+1,2)[node03596[42]-1];
__OME__tmp_listGen_1[43]=elementRaw(node04019,node03642_i+1,2)[node03596[43]-1];
__OME__tmp_listGen_1[44]=elementRaw(node04019,node03642_i+1,2)[node03596[44]-1];
__OME__tmp_listGen_1[45]=elementRaw(node04019,node03642_i+1,2)[node03596[45]-1];
__OME__tmp_listGen_1[46]=elementRaw(node04019,node03642_i+1,2)[node03596[46]-1];
__OME__tmp_listGen_1[47]=elementRaw(node04019,node03642_i+1,2)[node03596[47]-1];

__OME__sum_3= 0.0;
for(size_t __OME__tmpLoop_13_i=0;__OME__tmpLoop_13_i<48;++__OME__tmpLoop_13_i)
{

__OME__sum_3+=__OME__tmp_listGen_1[__OME__tmpLoop_13_i];
}
for(size_t __OME__tmpLoop_14_i=0;__OME__tmpLoop_14_i<696;++__OME__tmpLoop_14_i)
{

for(size_t __OME__tmpLoop_15_i=0;__OME__tmpLoop_15_i<224;++__OME__tmpLoop_15_i)
{

__OME__sum_5[__OME__tmpLoop_15_i]+=(*root.GetValueArchive())[node04019.GetModelIndex()+__OME__tmpLoop_15_i+224*(__OME__tmpLoop_14_i)];
}
}
OME_SCALAR __OME__sum_4;
OME_SCALAR __OME__tmp_listGen_2[168]={};
__OME__tmp_listGen_2[0]=elementRaw(node04019,node03642_i+1,2)[node03598[0]-1];
__OME__tmp_listGen_2[1]=elementRaw(node04019,node03642_i+1,2)[node03598[1]-1];
__OME__tmp_listGen_2[2]=elementRaw(node04019,node03642_i+1,2)[node03598[2]-1];
__OME__tmp_listGen_2[3]=elementRaw(node04019,node03642_i+1,2)[node03598[3]-1];
__OME__tmp_listGen_2[4]=elementRaw(node04019,node03642_i+1,2)[node03598[4]-1];
__OME__tmp_listGen_2[5]=elementRaw(node04019,node03642_i+1,2)[node03598[5]-1];
__OME__tmp_listGen_2[6]=elementRaw(node04019,node03642_i+1,2)[node03598[6]-1];
__OME__tmp_listGen_2[7]=elementRaw(node04019,node03642_i+1,2)[node03598[7]-1];
__OME__tmp_listGen_2[8]=elementRaw(node04019,node03642_i+1,2)[node03598[8]-1];
__OME__tmp_listGen_2[9]=elementRaw(node04019,node03642_i+1,2)[node03598[9]-1];
__OME__tmp_listGen_2[10]=elementRaw(node04019,node03642_i+1,2)[node03598[10]-1];
__OME__tmp_listGen_2[11]=elementRaw(node04019,node03642_i+1,2)[node03598[11]-1];
__OME__tmp_listGen_2[12]=elementRaw(node04019,node03642_i+1,2)[node03598[12]-1];
__OME__tmp_listGen_2[13]=elementRaw(node04019,node03642_i+1,2)[node03598[13]-1];
__OME__tmp_listGen_2[14]=elementRaw(node04019,node03642_i+1,2)[node03598[14]-1];
__OME__tmp_listGen_2[15]=elementRaw(node04019,node03642_i+1,2)[node03598[15]-1];
__OME__tmp_listGen_2[16]=elementRaw(node04019,node03642_i+1,2)[node03598[16]-1];
__OME__tmp_listGen_2[17]=elementRaw(node04019,node03642_i+1,2)[node03598[17]-1];
__OME__tmp_listGen_2[18]=elementRaw(node04019,node03642_i+1,2)[node03598[18]-1];
__OME__tmp_listGen_2[19]=elementRaw(node04019,node03642_i+1,2)[node03598[19]-1];
__OME__tmp_listGen_2[20]=elementRaw(node04019,node03642_i+1,2)[node03598[20]-1];
__OME__tmp_listGen_2[21]=elementRaw(node04019,node03642_i+1,2)[node03598[21]-1];
__OME__tmp_listGen_2[22]=elementRaw(node04019,node03642_i+1,2)[node03598[22]-1];
__OME__tmp_listGen_2[23]=elementRaw(node04019,node03642_i+1,2)[node03598[23]-1];
__OME__tmp_listGen_2[24]=elementRaw(node04019,node03642_i+1,2)[node03598[24]-1];
__OME__tmp_listGen_2[25]=elementRaw(node04019,node03642_i+1,2)[node03598[25]-1];
__OME__tmp_listGen_2[26]=elementRaw(node04019,node03642_i+1,2)[node03598[26]-1];
__OME__tmp_listGen_2[27]=elementRaw(node04019,node03642_i+1,2)[node03598[27]-1];
__OME__tmp_listGen_2[28]=elementRaw(node04019,node03642_i+1,2)[node03598[28]-1];
__OME__tmp_listGen_2[29]=elementRaw(node04019,node03642_i+1,2)[node03598[29]-1];
__OME__tmp_listGen_2[30]=elementRaw(node04019,node03642_i+1,2)[node03598[30]-1];
__OME__tmp_listGen_2[31]=elementRaw(node04019,node03642_i+1,2)[node03598[31]-1];
__OME__tmp_listGen_2[32]=elementRaw(node04019,node03642_i+1,2)[node03598[32]-1];
__OME__tmp_listGen_2[33]=elementRaw(node04019,node03642_i+1,2)[node03598[33]-1];
__OME__tmp_listGen_2[34]=elementRaw(node04019,node03642_i+1,2)[node03598[34]-1];
__OME__tmp_listGen_2[35]=elementRaw(node04019,node03642_i+1,2)[node03598[35]-1];
__OME__tmp_listGen_2[36]=elementRaw(node04019,node03642_i+1,2)[node03598[36]-1];
__OME__tmp_listGen_2[37]=elementRaw(node04019,node03642_i+1,2)[node03598[37]-1];
__OME__tmp_listGen_2[38]=elementRaw(node04019,node03642_i+1,2)[node03598[38]-1];
__OME__tmp_listGen_2[39]=elementRaw(node04019,node03642_i+1,2)[node03598[39]-1];
__OME__tmp_listGen_2[40]=elementRaw(node04019,node03642_i+1,2)[node03598[40]-1];
__OME__tmp_listGen_2[41]=elementRaw(node04019,node03642_i+1,2)[node03598[41]-1];
__OME__tmp_listGen_2[42]=elementRaw(node04019,node03642_i+1,2)[node03598[42]-1];
__OME__tmp_listGen_2[43]=elementRaw(node04019,node03642_i+1,2)[node03598[43]-1];
__OME__tmp_listGen_2[44]=elementRaw(node04019,node03642_i+1,2)[node03598[44]-1];
__OME__tmp_listGen_2[45]=elementRaw(node04019,node03642_i+1,2)[node03598[45]-1];
__OME__tmp_listGen_2[46]=elementRaw(node04019,node03642_i+1,2)[node03598[46]-1];
__OME__tmp_listGen_2[47]=elementRaw(node04019,node03642_i+1,2)[node03598[47]-1];
__OME__tmp_listGen_2[48]=elementRaw(node04019,node03642_i+1,2)[node03598[48]-1];
__OME__tmp_listGen_2[49]=elementRaw(node04019,node03642_i+1,2)[node03598[49]-1];
__OME__tmp_listGen_2[50]=elementRaw(node04019,node03642_i+1,2)[node03598[50]-1];
__OME__tmp_listGen_2[51]=elementRaw(node04019,node03642_i+1,2)[node03598[51]-1];
__OME__tmp_listGen_2[52]=elementRaw(node04019,node03642_i+1,2)[node03598[52]-1];
__OME__tmp_listGen_2[53]=elementRaw(node04019,node03642_i+1,2)[node03598[53]-1];
__OME__tmp_listGen_2[54]=elementRaw(node04019,node03642_i+1,2)[node03598[54]-1];
__OME__tmp_listGen_2[55]=elementRaw(node04019,node03642_i+1,2)[node03598[55]-1];
__OME__tmp_listGen_2[56]=elementRaw(node04019,node03642_i+1,2)[node03598[56]-1];
__OME__tmp_listGen_2[57]=elementRaw(node04019,node03642_i+1,2)[node03598[57]-1];
__OME__tmp_listGen_2[58]=elementRaw(node04019,node03642_i+1,2)[node03598[58]-1];
__OME__tmp_listGen_2[59]=elementRaw(node04019,node03642_i+1,2)[node03598[59]-1];
__OME__tmp_listGen_2[60]=elementRaw(node04019,node03642_i+1,2)[node03598[60]-1];
__OME__tmp_listGen_2[61]=elementRaw(node04019,node03642_i+1,2)[node03598[61]-1];
__OME__tmp_listGen_2[62]=elementRaw(node04019,node03642_i+1,2)[node03598[62]-1];
__OME__tmp_listGen_2[63]=elementRaw(node04019,node03642_i+1,2)[node03598[63]-1];
__OME__tmp_listGen_2[64]=elementRaw(node04019,node03642_i+1,2)[node03598[64]-1];
__OME__tmp_listGen_2[65]=elementRaw(node04019,node03642_i+1,2)[node03598[65]-1];
__OME__tmp_listGen_2[66]=elementRaw(node04019,node03642_i+1,2)[node03598[66]-1];
__OME__tmp_listGen_2[67]=elementRaw(node04019,node03642_i+1,2)[node03598[67]-1];
__OME__tmp_listGen_2[68]=elementRaw(node04019,node03642_i+1,2)[node03598[68]-1];
__OME__tmp_listGen_2[69]=elementRaw(node04019,node03642_i+1,2)[node03598[69]-1];
__OME__tmp_listGen_2[70]=elementRaw(node04019,node03642_i+1,2)[node03598[70]-1];
__OME__tmp_listGen_2[71]=elementRaw(node04019,node03642_i+1,2)[node03598[71]-1];
__OME__tmp_listGen_2[72]=elementRaw(node04019,node03642_i+1,2)[node03598[72]-1];
__OME__tmp_listGen_2[73]=elementRaw(node04019,node03642_i+1,2)[node03598[73]-1];
__OME__tmp_listGen_2[74]=elementRaw(node04019,node03642_i+1,2)[node03598[74]-1];
__OME__tmp_listGen_2[75]=elementRaw(node04019,node03642_i+1,2)[node03598[75]-1];
__OME__tmp_listGen_2[76]=elementRaw(node04019,node03642_i+1,2)[node03598[76]-1];
__OME__tmp_listGen_2[77]=elementRaw(node04019,node03642_i+1,2)[node03598[77]-1];
__OME__tmp_listGen_2[78]=elementRaw(node04019,node03642_i+1,2)[node03598[78]-1];
__OME__tmp_listGen_2[79]=elementRaw(node04019,node03642_i+1,2)[node03598[79]-1];
__OME__tmp_listGen_2[80]=elementRaw(node04019,node03642_i+1,2)[node03598[80]-1];
__OME__tmp_listGen_2[81]=elementRaw(node04019,node03642_i+1,2)[node03598[81]-1];
__OME__tmp_listGen_2[82]=elementRaw(node04019,node03642_i+1,2)[node03598[82]-1];
__OME__tmp_listGen_2[83]=elementRaw(node04019,node03642_i+1,2)[node03598[83]-1];
__OME__tmp_listGen_2[84]=elementRaw(node04019,node03642_i+1,2)[node03598[84]-1];
__OME__tmp_listGen_2[85]=elementRaw(node04019,node03642_i+1,2)[node03598[85]-1];
__OME__tmp_listGen_2[86]=elementRaw(node04019,node03642_i+1,2)[node03598[86]-1];
__OME__tmp_listGen_2[87]=elementRaw(node04019,node03642_i+1,2)[node03598[87]-1];
__OME__tmp_listGen_2[88]=elementRaw(node04019,node03642_i+1,2)[node03598[88]-1];
__OME__tmp_listGen_2[89]=elementRaw(node04019,node03642_i+1,2)[node03598[89]-1];
__OME__tmp_listGen_2[90]=elementRaw(node04019,node03642_i+1,2)[node03598[90]-1];
__OME__tmp_listGen_2[91]=elementRaw(node04019,node03642_i+1,2)[node03598[91]-1];
__OME__tmp_listGen_2[92]=elementRaw(node04019,node03642_i+1,2)[node03598[92]-1];
__OME__tmp_listGen_2[93]=elementRaw(node04019,node03642_i+1,2)[node03598[93]-1];
__OME__tmp_listGen_2[94]=elementRaw(node04019,node03642_i+1,2)[node03598[94]-1];
__OME__tmp_listGen_2[95]=elementRaw(node04019,node03642_i+1,2)[node03598[95]-1];
__OME__tmp_listGen_2[96]=elementRaw(node04019,node03642_i+1,2)[node03598[96]-1];
__OME__tmp_listGen_2[97]=elementRaw(node04019,node03642_i+1,2)[node03598[97]-1];
__OME__tmp_listGen_2[98]=elementRaw(node04019,node03642_i+1,2)[node03598[98]-1];
__OME__tmp_listGen_2[99]=elementRaw(node04019,node03642_i+1,2)[node03598[99]-1];
__OME__tmp_listGen_2[100]=elementRaw(node04019,node03642_i+1,2)[node03598[100]-1];
__OME__tmp_listGen_2[101]=elementRaw(node04019,node03642_i+1,2)[node03598[101]-1];
__OME__tmp_listGen_2[102]=elementRaw(node04019,node03642_i+1,2)[node03598[102]-1];
__OME__tmp_listGen_2[103]=elementRaw(node04019,node03642_i+1,2)[node03598[103]-1];
__OME__tmp_listGen_2[104]=elementRaw(node04019,node03642_i+1,2)[node03598[104]-1];
__OME__tmp_listGen_2[105]=elementRaw(node04019,node03642_i+1,2)[node03598[105]-1];
__OME__tmp_listGen_2[106]=elementRaw(node04019,node03642_i+1,2)[node03598[106]-1];
__OME__tmp_listGen_2[107]=elementRaw(node04019,node03642_i+1,2)[node03598[107]-1];
__OME__tmp_listGen_2[108]=elementRaw(node04019,node03642_i+1,2)[node03598[108]-1];
__OME__tmp_listGen_2[109]=elementRaw(node04019,node03642_i+1,2)[node03598[109]-1];
__OME__tmp_listGen_2[110]=elementRaw(node04019,node03642_i+1,2)[node03598[110]-1];
__OME__tmp_listGen_2[111]=elementRaw(node04019,node03642_i+1,2)[node03598[111]-1];
__OME__tmp_listGen_2[112]=elementRaw(node04019,node03642_i+1,2)[node03598[112]-1];
__OME__tmp_listGen_2[113]=elementRaw(node04019,node03642_i+1,2)[node03598[113]-1];
__OME__tmp_listGen_2[114]=elementRaw(node04019,node03642_i+1,2)[node03598[114]-1];
__OME__tmp_listGen_2[115]=elementRaw(node04019,node03642_i+1,2)[node03598[115]-1];
__OME__tmp_listGen_2[116]=elementRaw(node04019,node03642_i+1,2)[node03598[116]-1];
__OME__tmp_listGen_2[117]=elementRaw(node04019,node03642_i+1,2)[node03598[117]-1];
__OME__tmp_listGen_2[118]=elementRaw(node04019,node03642_i+1,2)[node03598[118]-1];
__OME__tmp_listGen_2[119]=elementRaw(node04019,node03642_i+1,2)[node03598[119]-1];
__OME__tmp_listGen_2[120]=elementRaw(node04019,node03642_i+1,2)[node03598[120]-1];
__OME__tmp_listGen_2[121]=elementRaw(node04019,node03642_i+1,2)[node03598[121]-1];
__OME__tmp_listGen_2[122]=elementRaw(node04019,node03642_i+1,2)[node03598[122]-1];
__OME__tmp_listGen_2[123]=elementRaw(node04019,node03642_i+1,2)[node03598[123]-1];
__OME__tmp_listGen_2[124]=elementRaw(node04019,node03642_i+1,2)[node03598[124]-1];
__OME__tmp_listGen_2[125]=elementRaw(node04019,node03642_i+1,2)[node03598[125]-1];
__OME__tmp_listGen_2[126]=elementRaw(node04019,node03642_i+1,2)[node03598[126]-1];
__OME__tmp_listGen_2[127]=elementRaw(node04019,node03642_i+1,2)[node03598[127]-1];
__OME__tmp_listGen_2[128]=elementRaw(node04019,node03642_i+1,2)[node03598[128]-1];
__OME__tmp_listGen_2[129]=elementRaw(node04019,node03642_i+1,2)[node03598[129]-1];
__OME__tmp_listGen_2[130]=elementRaw(node04019,node03642_i+1,2)[node03598[130]-1];
__OME__tmp_listGen_2[131]=elementRaw(node04019,node03642_i+1,2)[node03598[131]-1];
__OME__tmp_listGen_2[132]=elementRaw(node04019,node03642_i+1,2)[node03598[132]-1];
__OME__tmp_listGen_2[133]=elementRaw(node04019,node03642_i+1,2)[node03598[133]-1];
__OME__tmp_listGen_2[134]=elementRaw(node04019,node03642_i+1,2)[node03598[134]-1];
__OME__tmp_listGen_2[135]=elementRaw(node04019,node03642_i+1,2)[node03598[135]-1];
__OME__tmp_listGen_2[136]=elementRaw(node04019,node03642_i+1,2)[node03598[136]-1];
__OME__tmp_listGen_2[137]=elementRaw(node04019,node03642_i+1,2)[node03598[137]-1];
__OME__tmp_listGen_2[138]=elementRaw(node04019,node03642_i+1,2)[node03598[138]-1];
__OME__tmp_listGen_2[139]=elementRaw(node04019,node03642_i+1,2)[node03598[139]-1];
__OME__tmp_listGen_2[140]=elementRaw(node04019,node03642_i+1,2)[node03598[140]-1];
__OME__tmp_listGen_2[141]=elementRaw(node04019,node03642_i+1,2)[node03598[141]-1];
__OME__tmp_listGen_2[142]=elementRaw(node04019,node03642_i+1,2)[node03598[142]-1];
__OME__tmp_listGen_2[143]=elementRaw(node04019,node03642_i+1,2)[node03598[143]-1];
__OME__tmp_listGen_2[144]=elementRaw(node04019,node03642_i+1,2)[node03598[144]-1];
__OME__tmp_listGen_2[145]=elementRaw(node04019,node03642_i+1,2)[node03598[145]-1];
__OME__tmp_listGen_2[146]=elementRaw(node04019,node03642_i+1,2)[node03598[146]-1];
__OME__tmp_listGen_2[147]=elementRaw(node04019,node03642_i+1,2)[node03598[147]-1];
__OME__tmp_listGen_2[148]=elementRaw(node04019,node03642_i+1,2)[node03598[148]-1];
__OME__tmp_listGen_2[149]=elementRaw(node04019,node03642_i+1,2)[node03598[149]-1];
__OME__tmp_listGen_2[150]=elementRaw(node04019,node03642_i+1,2)[node03598[150]-1];
__OME__tmp_listGen_2[151]=elementRaw(node04019,node03642_i+1,2)[node03598[151]-1];
__OME__tmp_listGen_2[152]=elementRaw(node04019,node03642_i+1,2)[node03598[152]-1];
__OME__tmp_listGen_2[153]=elementRaw(node04019,node03642_i+1,2)[node03598[153]-1];
__OME__tmp_listGen_2[154]=elementRaw(node04019,node03642_i+1,2)[node03598[154]-1];
__OME__tmp_listGen_2[155]=elementRaw(node04019,node03642_i+1,2)[node03598[155]-1];
__OME__tmp_listGen_2[156]=elementRaw(node04019,node03642_i+1,2)[node03598[156]-1];
__OME__tmp_listGen_2[157]=elementRaw(node04019,node03642_i+1,2)[node03598[157]-1];
__OME__tmp_listGen_2[158]=elementRaw(node04019,node03642_i+1,2)[node03598[158]-1];
__OME__tmp_listGen_2[159]=elementRaw(node04019,node03642_i+1,2)[node03598[159]-1];
__OME__tmp_listGen_2[160]=elementRaw(node04019,node03642_i+1,2)[node03598[160]-1];
__OME__tmp_listGen_2[161]=elementRaw(node04019,node03642_i+1,2)[node03598[161]-1];
__OME__tmp_listGen_2[162]=elementRaw(node04019,node03642_i+1,2)[node03598[162]-1];
__OME__tmp_listGen_2[163]=elementRaw(node04019,node03642_i+1,2)[node03598[163]-1];
__OME__tmp_listGen_2[164]=elementRaw(node04019,node03642_i+1,2)[node03598[164]-1];
__OME__tmp_listGen_2[165]=elementRaw(node04019,node03642_i+1,2)[node03598[165]-1];
__OME__tmp_listGen_2[166]=elementRaw(node04019,node03642_i+1,2)[node03598[166]-1];
__OME__tmp_listGen_2[167]=elementRaw(node04019,node03642_i+1,2)[node03598[167]-1];

__OME__sum_4= 0.0;
for(size_t __OME__tmpLoop_16_i=0;__OME__tmpLoop_16_i<168;++__OME__tmpLoop_16_i)
{

__OME__sum_4+=__OME__tmp_listGen_2[__OME__tmpLoop_16_i];
}
OME_SCALAR __OME__sum_6;
__OME__sum_6= 0.0;
for(size_t __OME__tmpLoop_17_i=0;__OME__tmpLoop_17_i<224;++__OME__tmpLoop_17_i)
{

__OME__sum_6+=__OME__sum_5[__OME__tmpLoop_17_i];
}
for(size_t __OME__tmpLoop_19_i=0;__OME__tmpLoop_19_i<5;++__OME__tmpLoop_19_i)
{

__OME__sum_7[__OME__tmpLoop_19_i]+=(*root.GetValueArchive())[node03896.GetModelIndex()+__OME__tmpLoop_19_i+5*(node03642_i)];
}
//=-=-=-=-=-=-=-=-=-= node03862 (Non_white_pop)
node03862.Reset(TestModelBm);
node03862=(OME_SCALAR)__OME__sum_4/__OME__sum_6;
node03862.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03834 (Children)
node03834.Reset(TestModelBm);
node03834=(OME_SCALAR)__OME__sum_3;
node03834.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03836 (Elderly)
node03836.Reset(TestModelBm);
node03836=(OME_SCALAR)__OME__sum_2;
node03836.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03830 (Empl)
node03830.Reset(TestModelBm);
node03830=(OME_SCALAR)elementRaw(node04015,node03642_i+1,1);
node03830.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03826 (Wealth)
node03826.Reset(TestModelBm);
node03826=(OME_SCALAR)elementRaw(node04023,node03642_i+1,1);
node03826.BatchInitialize(TestModelTime,TestModelBm);

OME_SCALAR __OME__tempArray_10[]={node03834,node03836,node03862,node03826,node03830};
size_t __OME__tempArray_10_dims[]={5};
//=-=-=-=-=-=-=-=-=-= node03853 (Social_Fabric_Display)
node03853.Reset(TestModelBm);
node03853=(OME_SCALAR)element((OME_SCALAR*)__OME__tempArray_10,__OME__tempArray_10_dims,node03695,1);
node03853.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03828 (Rural)
node03828.Reset(TestModelBm);
node03828=(OME_SCALAR)elementRaw(node04013,node03642_i+1,1);
node03828.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03872 (blgr_ID)
node03872.Reset(TestModelBm);
node03872=(OME_SCALAR)element(node03641,node03642_i+1,1);
node03872.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03844 (Complaints_Index)
node03844.Reset(TestModelBm);
node03844=(OME_SCALAR)element(node03896,2.0,1);
node03844.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03848 (EMS_Index)
node03848.Reset(TestModelBm);
node03848=(OME_SCALAR)element(node03896,1.0,1);
node03848.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03825 (Heat_Stress_Index)
node03825.Reset(TestModelBm);
node03825.BatchInitialize(TestModelTime,TestModelBm);

}
node03642.SetCurrentInstance(0);

for(size_t node03611_i=0;node03611_i<6;++node03611_i)
{
node03611.SetCurrentInstance(node03611_i);

//=-=-=-=-=-=-=-=-=-= node03800 (diffusion_rt)
node03800.Reset(TestModelBm);
node03800=(OME_SCALAR)1.0;
node03800.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03797 (Global_concentration)
node03797.Reset(TestModelBm);
node03797=(OME_SCALAR)0.35;
node03797.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03794 (Wind_sensitive)
node03794.Reset(TestModelBm);
node03794=(OME_SCALAR)0.0;
node03794.BatchInitialize(TestModelTime,TestModelBm);

OME_SCALAR __OME__tempArray_12[]={5.64,1276.4,8986.57,8986.57,2199.92,6000.11};
size_t __OME__tempArray_12_dims[]={6};
//=-=-=-=-=-=-=-=-=-= node03790 (pollutant_removal_unit_value)
node03790.Reset(TestModelBm);
node03790=(OME_SCALAR)element((OME_SCALAR*)__OME__tempArray_12,__OME__tempArray_12_dims,node03611_i+1,1);
node03790.BatchInitialize(TestModelTime,TestModelBm);

OME_SCALAR __OME__tempArray_13[]={9812.19,2.47466,10.017,38.7003,5.47233,25.8955};
size_t __OME__tempArray_13_dims[]={6};
//=-=-=-=-=-=-=-=-=-= node03788 (__pollutant_removal_rate)
node03788.Reset(TestModelBm);
node03788=(OME_SCALAR)element((OME_SCALAR*)__OME__tempArray_13,__OME__tempArray_13_dims,node03611_i+1,1)/365.0;
node03788.BatchInitialize(TestModelTime,TestModelBm);

}
node03611.SetCurrentInstance(0);

for(size_t node03655_i=0;node03655_i<8;++node03655_i)
{
node03655.SetCurrentInstance(node03655_i);

for(size_t node04090_i=0;node04090_i<27;++node04090_i)
{
node04090.SetCurrentInstance(node04090_i);

for(size_t makeArray_node040911_i=0;makeArray_node040911_i<9;++makeArray_node040911_i)
{

(*root.GetValueArchive())[node04091.GetModelIndex()+makeArray_node040911_i+9*(node04090_i+27*(node03655_i))]=(OME_SCALAR)1.0;

}
//=-=-=-=-=-=-=-=-=-= node04091 (NDVI_data)
node04091.BatchInitialize(TestModelTime,TestModelBm);

}
node04090.SetCurrentInstance(0);

}
node03655.SetCurrentInstance(0);

for(size_t node03608_i=0;node03608_i<365;++node03608_i)
{
node03608.SetCurrentInstance(node03608_i);

for(size_t node03776_i=0;node03776_i<40;++node03776_i)
{
node03776.SetCurrentInstance(node03776_i);

//=-=-=-=-=-=-=-=-=-= node03786 (PREC)
node03786.Reset(TestModelBm);
node03786.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03784 (Humidity)
node03784.Reset(TestModelBm);
node03784.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03783 (Temp_C)
node03783.Reset(TestModelBm);
node03783.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03781 (Windspeed)
node03781.Reset(TestModelBm);
node03781=(OME_SCALAR)0.0;
node03781.BatchInitialize(TestModelTime,TestModelBm);

}
node03776.SetCurrentInstance(0);

}
node03608.SetCurrentInstance(0);

for(size_t node03593_i=0;node03593_i<39;++node03593_i)
{
node03593.SetCurrentInstance(node03593_i);

for(size_t node03732_i=0;node03732_i<12;++node03732_i)
{
node03732.SetCurrentInstance(node03732_i);

//=-=-=-=-=-=-=-=-=-= node03734 (Texas_Water_Development_Board)
node03734.Reset(TestModelBm);
node03734.BatchInitialize(TestModelTime,TestModelBm);

}
node03732.SetCurrentInstance(0);

}
node03593.SetCurrentInstance(0);

for(size_t node03595_i=0;node03595_i<10;++node03595_i)
{
node03595.SetCurrentInstance(node03595_i);

for(size_t node03739_i=0;node03739_i<224;++node03739_i)
{
node03739.SetCurrentInstance(node03739_i);

for(size_t node03744_i=0;node03744_i<11;++node03744_i)
{
node03744.SetCurrentInstance(node03744_i);

//=-=-=-=-=-=-=-=-=-= node03746 (Race_Eth_Gen_Age_Vulnerability)
node03746.Reset(TestModelBm);
node03746.BatchInitialize(TestModelTime,TestModelBm);

}
node03744.SetCurrentInstance(0);

}
node03739.SetCurrentInstance(0);

for(size_t node03740_i=0;node03740_i<2;++node03740_i)
{
node03740.SetCurrentInstance(node03740_i);
node03741.SetCurrentInstance(node03740_i);

for(size_t node03749_i=0;node03749_i<11;++node03749_i)
{
node03749.SetCurrentInstance(node03749_i);
node03753.SetCurrentInstance(node03749_i);

//=-=-=-=-=-=-=-=-=-= node03751 (Employment_Vulnerability)
node03751.Reset(TestModelBm);
node03751.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03755 (Life_Style_Vulnerability)
node03755.Reset(TestModelBm);
node03755.BatchInitialize(TestModelTime,TestModelBm);

}
node03749.SetCurrentInstance(0);
node03753.SetCurrentInstance(0);

}
node03740.SetCurrentInstance(0);
node03741.SetCurrentInstance(0);

for(size_t node03743_i=0;node03743_i<16;++node03743_i)
{
node03743.SetCurrentInstance(node03743_i);

for(size_t node03756_i=0;node03756_i<11;++node03756_i)
{
node03756.SetCurrentInstance(node03756_i);

//=-=-=-=-=-=-=-=-=-= node03759 (Wealth_Vulnerability)
node03759.Reset(TestModelBm);
node03759.BatchInitialize(TestModelTime,TestModelBm);

}
node03756.SetCurrentInstance(0);

}
node03743.SetCurrentInstance(0);

}
node03595.SetCurrentInstance(0);

for(size_t __OME__tmpLoop_20_i=0;__OME__tmpLoop_20_i<5;++__OME__tmpLoop_20_i)
{

//=-=-=-=-=-=-=-=-=-= node03594 (Total_Mortalities)
(*root.GetValueArchive())[node03594.GetModelIndex()+__OME__tmpLoop_20_i]=(OME_SCALAR)__OME__sum_7[__OME__tmpLoop_20_i];
node03594.BatchInitialize(TestModelTime,TestModelBm);

}
//=-=-=-=-=-=-=-=-=-= node03614 (Biweely_period)
node03614.Reset(TestModelBm);
node03614=(OME_SCALAR)__OME__firsttrue_1;
node03614.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03605 (use_NDVI_data)
node03605.Reset(TestModelBm);
node03605=(OME_SCALAR)0.0;
node03605.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03712 (NDVI_data_or_predictions)
node03712.Reset(TestModelBm);
node03712=(OME_SCALAR)(node03622<32.0||node03622>=40.0||node03622==39.0&&node03614>19.0 ? (OME_SCALAR)1.0 : (OME_SCALAR)((node03605==0.0 ? (OME_SCALAR)0.0 : (OME_SCALAR)1.0)));
node03712.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03697 (NDVI_year)
node03697.Reset(TestModelBm);
node03697=(OME_SCALAR)node03622-31.0;
node03697.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03709 (Averaged_RA_Temp)
node03709.Reset(TestModelBm);
node03709=(OME_SCALAR)interpTable(node03709,TestModelBm,(node03638));
node03709.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03672 (temp_limit_sizer)
node03672.Reset(TestModelBm);
node03672=(OME_SCALAR)1.0;
node03672.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03680 (Init_RH)
node03680.Reset(TestModelBm);
node03680=(OME_SCALAR)node03682*66.2;
node03680.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03663 (RH)
node03663.Reset(TestModelBm);
node03663=(OME_SCALAR)node03680;
node03663.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03685 (RA_Rel_Hum)
node03685.Reset(TestModelBm);
node03685=(OME_SCALAR)node03663/(node03682+1.0);
node03685.BatchInitialize(TestModelTime,TestModelBm);

for(size_t __OME__tmpLoop_21_i=0;__OME__tmpLoop_21_i<9;++__OME__tmpLoop_21_i)
{
node03711.SetCurrentInstance(__OME__tmpLoop_21_i);

//=-=-=-=-=-=-=-=-=-= node04124 (Temp_limit_coeff)
node04124.Reset(TestModelBm);
node04124.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04123 (Opt_temp)
node04123.Reset(TestModelBm);
node04123.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04119 (Temp_limit)
node04119.Reset(TestModelBm);
node04119=(OME_SCALAR)OMEMin((0.0),(node04123-node03683));
node04119.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04122 (YA_T_slope)
node04122.Reset(TestModelBm);
node04122.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04121 (RH14_slope)
node04121.Reset(TestModelBm);
node04121.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04118 (Intercept)
node04118.Reset(TestModelBm);
node04118.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04117 (T14_slope)
node04117.Reset(TestModelBm);
node04117.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04115 (NDVI_predicted)
node04115.Reset(TestModelBm);
node04115=(OME_SCALAR)node04118+node04122*node03709+node04117*node03683+node04121*node03685+node04124*node03672*node04119;
node04115.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03699 (NDVIs)
(*root.GetValueArchive())[node03699.GetModelIndex()+__OME__tmpLoop_21_i]=(OME_SCALAR)(node03712==1.0 ? (OME_SCALAR)0.0 : (OME_SCALAR)element(element(node04091,node03697,3),node03614)[__OME__tmpLoop_21_i]);
node03699.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04130 (NDVI)
node04130.Reset(TestModelBm);
node04130=(OME_SCALAR)(node03712==0.0 ? (OME_SCALAR)element(node03699,__OME__tmpLoop_21_i+1,1) : (OME_SCALAR)node04115);
node04130.BatchInitialize(TestModelTime,TestModelBm);

}
node03711.SetCurrentInstance(0);

//=-=-=-=-=-=-=-=-=-= node03671 (Data_or_GCC_scenario)
node03671.Reset(TestModelBm);
node03671=(OME_SCALAR)0.0;
node03671.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03612 (Choice_of_weather_data)
node03612.Reset(TestModelBm);
node03612=(OME_SCALAR)(node03622<27.0 ? (OME_SCALAR)0.0 : (OME_SCALAR)((node03622<41.0&&node03671==0.0 ? (OME_SCALAR)0.0 : (OME_SCALAR)1.0)));
node03612.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03653 (GCC_D_Temp)
node03653.Reset(TestModelBm);
node03653=(OME_SCALAR)82.0;
node03653.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03604 (Air_T)
node03604.Reset(TestModelBm);
node03604=(OME_SCALAR)(currenttime(TestModelBm)==0.0 ? (OME_SCALAR)0.0 : (OME_SCALAR)((node03612==0.0 ? (OME_SCALAR)element(element(node03783,node03638,2),node03622) : (OME_SCALAR)node03653))+4.55);
node03604.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03632 (Bare_Soil_Temperature)
node03632.Reset(TestModelBm);
node03632=(OME_SCALAR)(node03604-3.1876)/0.6443;
node03632.BatchInitialize(TestModelTime,TestModelBm);

for(size_t node03627_i=0;node03627_i<15;++node03627_i)
{
node03627.SetCurrentInstance(node03627_i);

__OME__sum_32[node03627_i]= 0.0;
__OME__sum_31[node03627_i]= 0.0;
__OME__sum_30[node03627_i]= 0.0;
//=-=-=-=-=-=-=-=-=-= node03804 (Temperature_buffer_capacity)
node03804.Reset(TestModelBm);
node03804=(OME_SCALAR)(node03627_i+1==1.0 ? (OME_SCALAR)node03607 : (OME_SCALAR)0.0);
node03804.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03823 (Air_T_intercept)
node03823.Reset(TestModelBm);
node03823=(OME_SCALAR)10.0275;
node03823.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03821 (Boundary_Layer_Heat_Conductance)
node03821.Reset(TestModelBm);
node03821.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03973 (Net_radiant_Energy)
node03973.Reset(TestModelBm);
node03973=(OME_SCALAR)node03821*29.17*(node03632-node03604);
node03973.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03817 (Air_T_slope)
node03817.Reset(TestModelBm);
node03817=(OME_SCALAR)0.5309;
node03817.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03814 (Tree_cover)
node03814.Reset(TestModelBm);
node03814.BatchInitialize(TestModelTime,TestModelBm);

OME_SCALAR __OME__tempArray_11[]={0.0,0.9,0.65,0.35,0.1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
size_t __OME__tempArray_11_dims[]={15};
//=-=-=-=-=-=-=-=-=-= node03812 (Pervious)
node03812.Reset(TestModelBm);
node03812=(OME_SCALAR)element((OME_SCALAR*)__OME__tempArray_11,__OME__tempArray_11_dims,node03627_i+1,1);
node03812.BatchInitialize(TestModelTime,TestModelBm);

OME_SCALAR __OME__tempArray_14[]={0.0,node03814*elementRaw(node04130,2.0,1)+(node03812-node03814)*elementRaw(node04130,5.0,1),node03814*elementRaw(node04130,2.0,1)+(node03812-node03814)*elementRaw(node04130,5.0,1),node03814*elementRaw(node04130,2.0,1)+(node03812-node03814)*elementRaw(node04130,5.0,1),node03814*elementRaw(node04130,2.0,1)+(node03812-node03814)*elementRaw(node04130,5.0,1),0.06,elementRaw(node04130,1.0,1),elementRaw(node04130,2.0,1),elementRaw(node04130,3.0,1),elementRaw(node04130,4.0,1),elementRaw(node04130,5.0,1),elementRaw(node04130,6.0,1),elementRaw(node04130,7.0,1),elementRaw(node04130,8.0,1),elementRaw(node04130,9.0,1)};
size_t __OME__tempArray_14_dims[]={15};
//=-=-=-=-=-=-=-=-=-= node03820 (NDVI)
node03820.Reset(TestModelBm);
node03820=(OME_SCALAR)element((OME_SCALAR*)__OME__tempArray_14,__OME__tempArray_14_dims,node03627_i+1,1);
node03820.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03809 (Effect_on_wind)
node03809.Reset(TestModelBm);
node03809=(OME_SCALAR)0.0;
node03809.BatchInitialize(TestModelTime,TestModelBm);

}
node03627.SetCurrentInstance(0);

//=-=-=-=-=-=-=-=-=-= node03651 (GCC_D_Relative_Humidity)
node03651.Reset(TestModelBm);
node03651=(OME_SCALAR)0.5;
node03651.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03620 (R_Humidity)
node03620.Reset(TestModelBm);
node03620=(OME_SCALAR)(currenttime(TestModelBm)==0.0 ? (OME_SCALAR)0.0 : (OME_SCALAR)((node03612==0.0 ? (OME_SCALAR)element(element(node03784,node03638,2),node03622) : (OME_SCALAR)node03651)));
node03620.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03634 (Saturated_pressure)
node03634.Reset(TestModelBm);
node03634=(OME_SCALAR)0.61078*OMECFuncs::exp(17.269*node03632/(237.3+node03632));
node03634.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03628 (VPD)
node03628.Reset(TestModelBm);
node03628=(OME_SCALAR)node03634*(1.0-node03620/100.0);
node03628.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03630 (temp_reduction_per_unit_of_NDVI)
node03630.Reset(TestModelBm);
node03630=(OME_SCALAR)39.69*node03628/(12.21+node03628);
node03630.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node00228 (Total_area)
node00228.Reset(TestModelBm);
node00228=(OME_SCALAR)__OME__sum_0;
node00228.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03602 (total_population)
node03602.Reset(TestModelBm);
node03602=(OME_SCALAR)__OME__sum_1;
node03602.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node00059 (Choose_Heat_wave_definition)
node00059.Reset(TestModelBm);
node00059=(OME_SCALAR)2.0;
node00059.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node00062 (Numeric_Heat_effect_mortality)
node00062.Reset(TestModelBm);
node00062=(OME_SCALAR)0.023;
node00062.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03637 (Choose_LandCover)
node03637.BatchInitialize(TestModelTime,TestModelBm);
node03637.Evaluate(TestModelTime,TestModelBm);

for(size_t node03644_i=0;node03644_i<696;++node03644_i)
{
node03644.SetCurrentInstance(node03644_i);

for(size_t node03988_i=0;node03988_i<15;++node03988_i)
{
node03988.SetCurrentInstance(node03988_i);

//=-=-=-=-=-=-=-=-=-= node04039 (NDVI)
node04039.Reset(TestModelBm);
node04039=(OME_SCALAR)elementRaw(node03820,node03988_i+1,1);
node04039.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04041 (weighted_NDVI)
node04041.Reset(TestModelBm);
node04041=(OME_SCALAR)node04039*node04060;
node04041.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04034 (Effect_on_wind)
node04034.Reset(TestModelBm);
node04034=(OME_SCALAR)node04060*elementRaw(node03809,node03988_i+1,1);
node04034.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04030 (Tree_cover_area)
node04030.Reset(TestModelBm);
node04030=(OME_SCALAR)node04060*node03970*elementRaw(node03814,node03988_i+1,1);
node04030.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04052 (temperature_reduction)
node04052.Reset(TestModelBm);
node04052=(OME_SCALAR)node03630*elementRaw(node03820,node03988_i+1,1)+elementRaw(node03804,node03988_i+1,1);
node04052.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04054 (Canopy_temp)
node04054.Reset(TestModelBm);
node04054=(OME_SCALAR)node03632-node04052;
node04054.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04064 (sensible_heat_flux)
node04064.Reset(TestModelBm);
node04064=(OME_SCALAR)elementRaw(node03821,node03988_i+1,1)*29.17*(node04054-node03604);
node04064.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04056 (evaporation)
node04056.Reset(TestModelBm);
node04056=(OME_SCALAR)(elementRaw(node03973,node03988_i+1,1)-node04064-node04043)/2257.0;
node04056.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04046 (Air_temp)
node04046.Reset(TestModelBm);
node04046=(OME_SCALAR)elementRaw(node03823,node03988_i+1,1)+elementRaw(node03817,node03988_i+1,1)*node04054;
node04046.BatchInitialize(TestModelTime,TestModelBm);

}
node03988.SetCurrentInstance(0);

OME_SCALAR __OME__sum_14;
__OME__sum_14= 0.0;
OME_SCALAR __OME__sum_12;
__OME__sum_12= 0.0;
OME_SCALAR __OME__sum_10;
__OME__sum_10= 0.0;
OME_SCALAR __OME__sum_9;
__OME__sum_9= 0.0;
OME_SCALAR __OME__posgreatest_0;
__OME__posgreatest_0=0;
OME_SCALAR __OME__posgreatest_0_second=OME_SCALAR_MIN;
OME_SCALAR __OME__sum_8;
__OME__sum_8= 0.0;
for(size_t __OME__tmpLoop_22_i=0;__OME__tmpLoop_22_i<15;++__OME__tmpLoop_22_i)
{

__OME__sum_8+=(*root.GetValueArchive())[node04056.GetModelIndex()+__OME__tmpLoop_22_i+15*(node03644_i)]*(*root.GetValueArchive())[node04060.GetModelIndex()+__OME__tmpLoop_22_i+15*(node03644_i)];
if(__OME__posgreatest_0_second < (*root.GetValueArchive())[node04060.GetModelIndex()+__OME__tmpLoop_22_i+15*(node03644_i)]) { __OME__posgreatest_0=__OME__tmpLoop_22_i+1; __OME__posgreatest_0_second=(*root.GetValueArchive())[node04060.GetModelIndex()+__OME__tmpLoop_22_i+15*(node03644_i)];}
__OME__sum_9+=(*root.GetValueArchive())[node04030.GetModelIndex()+__OME__tmpLoop_22_i+15*(node03644_i)];
__OME__sum_10+=(*root.GetValueArchive())[node04041.GetModelIndex()+__OME__tmpLoop_22_i+15*(node03644_i)];
__OME__sum_12+=(*root.GetValueArchive())[node04054.GetModelIndex()+__OME__tmpLoop_22_i+15*(node03644_i)]*(*root.GetValueArchive())[node04060.GetModelIndex()+__OME__tmpLoop_22_i+15*(node03644_i)];
__OME__sum_14+=(*root.GetValueArchive())[node04046.GetModelIndex()+__OME__tmpLoop_22_i+15*(node03644_i)]*(*root.GetValueArchive())[node04060.GetModelIndex()+__OME__tmpLoop_22_i+15*(node03644_i)];
}
//=-=-=-=-=-=-=-=-=-= node03975 (Average_air_temperatures)
node03975.Reset(TestModelBm);
node03975=(OME_SCALAR)__OME__sum_14;
node03975.BatchInitialize(TestModelTime,TestModelBm);

OME_SCALAR __OME__sum_11;
__OME__sum_11= 0.0;
for(size_t __OME__tmpLoop_26_i=0;__OME__tmpLoop_26_i<1;++__OME__tmpLoop_26_i)
{
node00017.SetCurrentInstance(__OME__tmpLoop_26_i);
node03967.SetCurrentInstance(__OME__tmpLoop_26_i);

OME_SCALAR __OME__firsttrue_3;
__OME__firsttrue_3=0;
for(size_t __OME__tmpLoop_30_i=0;__OME__tmpLoop_30_i<16;++__OME__tmpLoop_30_i)
{

if(!__OME__firsttrue_3 && node04023<(*root.GetValueArchive())[node03674.GetModelIndex()+__OME__tmpLoop_30_i]) __OME__firsttrue_3=__OME__tmpLoop_30_i+1;
}
//=-=-=-=-=-=-=-=-=-= node04024 (Wealth_position)
node04024.Reset(TestModelBm);
node04024=(OME_SCALAR)__OME__firsttrue_3;
node04024.BatchInitialize(TestModelTime,TestModelBm);

__OME__sum_11+=(*root.GetValueArchive())[node04150.GetModelIndex()];
//=-=-=-=-=-=-=-=-=-= arc01739 (reset_to_0)
arc01739.Reset(TestModelBm);
arc01739=(OME_SCALAR)(node03638==273.0 ? (OME_SCALAR)node00052-1.0 : (OME_SCALAR)0.0);
arc01739.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node00021 (Heat_wave_temperatures)
node00021.Reset(TestModelBm);
node00021=(OME_SCALAR)(node03975>node00045 ? (OME_SCALAR)1.0 : (OME_SCALAR)0.0);
node00021.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= arc01510 (add_time)
arc01510.Reset(TestModelBm);
arc01510=(OME_SCALAR)node00021;
arc01510.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= arc01406 (reset_intensity)
arc01406.Reset(TestModelBm);
arc01406=(OME_SCALAR)(1.0-node00021)*node00024;
arc01406.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= arc01203 (Add_High_tempreatures)
arc01203.Reset(TestModelBm);
arc01203=(OME_SCALAR)node00021*OMEMax((0.0),(node03975-node00045));
arc01203.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node00047 (Heat_wave_starts)
node00047.Reset(TestModelBm);
node00047=(OME_SCALAR)(lookupInit_last(TestModelBm,node00021)==0.0&&node00021==1.0 ? (OME_SCALAR)node03638-121.0 : (OME_SCALAR)366.0);
node00047.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= arc01511 (end_of_period)
arc01511.Reset(TestModelBm);
arc01511=(OME_SCALAR)(1.0-node00021)*node00030;
arc01511.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= arc01738 (Next_wave)
arc01738.Reset(TestModelBm);
arc01738=(OME_SCALAR)(arc01511>0.0 ? (OME_SCALAR)1.0 : (OME_SCALAR)0.0);
arc01738.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node00034 (_Heat_effect)
node00034.Reset(TestModelBm);
node00034=(OME_SCALAR)OMEMax((0.0),(node03975-node00045))/5.2;
node00034.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node00036 (A_and_B_HW)
node00036.Reset(TestModelBm);
node00036=(OME_SCALAR)node00048*OMEMax((node00030),(2.0))+node00052;
node00036.BatchInitialize(TestModelTime,TestModelBm);

}
node00017.SetCurrentInstance(0);
node03967.SetCurrentInstance(0);

OME_SCALAR __OME__sum_13;
__OME__sum_13= 0.0;
for(size_t __OME__tmpLoop_28_i=0;__OME__tmpLoop_28_i<6;++__OME__tmpLoop_28_i)
{

__OME__sum_13+=(*root.GetValueArchive())[node04081.GetModelIndex()+__OME__tmpLoop_28_i+6*(node03644_i)];
}
OME_SCALAR __OME__sum_24[11];
size_t __OME__sum_24_dims[]={11};
OME_SCALAR __OME__sum_23[2][11];
size_t __OME__sum_23_dims[]={2,11};
OME_SCALAR __OME__sum_21[11];
size_t __OME__sum_21_dims[]={11};
OME_SCALAR __OME__sum_20[2][11];
size_t __OME__sum_20_dims[]={2,11};
OME_SCALAR __OME__sum_18[16][11];
size_t __OME__sum_18_dims[]={16,11};
OME_SCALAR __OME__sum_16[11];
size_t __OME__sum_16_dims[]={11};
OME_SCALAR __OME__sum_15[224][11];
size_t __OME__sum_15_dims[]={224,11};
for(size_t __OME__iniLoop_6_i=0;__OME__iniLoop_6_i<11;++__OME__iniLoop_6_i)
{

for(size_t __OME__iniLoop_8_i=0;__OME__iniLoop_8_i<2;++__OME__iniLoop_8_i)
{

__OME__sum_23[__OME__iniLoop_8_i][__OME__iniLoop_6_i]= 0.0;
__OME__sum_20[__OME__iniLoop_8_i][__OME__iniLoop_6_i]= 0.0;
}
for(size_t __OME__iniLoop_13_i=0;__OME__iniLoop_13_i<16;++__OME__iniLoop_13_i)
{

__OME__sum_18[__OME__iniLoop_13_i][__OME__iniLoop_6_i]= 0.0;
}
for(size_t __OME__iniLoop_16_i=0;__OME__iniLoop_16_i<224;++__OME__iniLoop_16_i)
{

__OME__sum_15[__OME__iniLoop_16_i][__OME__iniLoop_6_i]= 0.0;
}
__OME__sum_24[__OME__iniLoop_6_i]= 0.0;
__OME__sum_21[__OME__iniLoop_6_i]= 0.0;
__OME__sum_16[__OME__iniLoop_6_i]= 0.0;
}
//=-=-=-=-=-=-=-=-=-= node03954 (T_Air_pollution)
node03954.Reset(TestModelBm);
node03954=(OME_SCALAR)__OME__sum_13;
node03954.BatchInitialize(TestModelTime,TestModelBm);

OME_SCALAR __OME__tempArray_15[]={node00036,node00034};
size_t __OME__tempArray_15_dims[]={2};
//=-=-=-=-=-=-=-=-=-= node03965 (Heat_Wave)
node03965.Reset(TestModelBm);
node03965=(OME_SCALAR)element((OME_SCALAR*)__OME__tempArray_15,__OME__tempArray_15_dims,node00059,1);
node03965.BatchInitialize(TestModelTime,TestModelBm);

OME_SCALAR __OME__tempArray_16[]={node03965,0.0,node03954,0.0,0.0,0.0,0.0,0.0,0.0,1.0};
size_t __OME__tempArray_16_dims[]={10};
for(size_t __OME__tmpLoop_31_i=0;__OME__tmpLoop_31_i<10;++__OME__tmpLoop_31_i)
{

//=-=-=-=-=-=-=-=-=-= node03961 (Hazards)
(*root.GetValueArchive())[node03961.GetModelIndex()+__OME__tmpLoop_31_i+10*(node03644_i)]=(OME_SCALAR)__OME__tempArray_16[__OME__tmpLoop_31_i];
node03961.BatchInitialize(TestModelTime,TestModelBm);

}
for(size_t __OME__tmpLoop_32_i=0;__OME__tmpLoop_32_i<10;++__OME__tmpLoop_32_i)
{

for(size_t __OME__tmpLoop_33_i=0;__OME__tmpLoop_33_i<224;++__OME__tmpLoop_33_i)
{

for(size_t __OME__tmpLoop_34_i=0;__OME__tmpLoop_34_i<11;++__OME__tmpLoop_34_i)
{

__OME__sum_15[__OME__tmpLoop_33_i][__OME__tmpLoop_34_i]+=(*root.GetValueArchive())[node03961.GetModelIndex()+__OME__tmpLoop_32_i+10*(node03644_i)]*(*root.GetValueArchive())[node03746.GetModelIndex()+__OME__tmpLoop_34_i+11*(__OME__tmpLoop_33_i+224*(__OME__tmpLoop_32_i))];
}
}
for(size_t __OME__tmpLoop_38_i=0;__OME__tmpLoop_38_i<16;++__OME__tmpLoop_38_i)
{

for(size_t __OME__tmpLoop_39_i=0;__OME__tmpLoop_39_i<11;++__OME__tmpLoop_39_i)
{

__OME__sum_18[__OME__tmpLoop_38_i][__OME__tmpLoop_39_i]+=(*root.GetValueArchive())[node03961.GetModelIndex()+__OME__tmpLoop_32_i+10*(node03644_i)]*(*root.GetValueArchive())[node03759.GetModelIndex()+__OME__tmpLoop_39_i+11*(__OME__tmpLoop_38_i+16*(__OME__tmpLoop_32_i))];
}
}
for(size_t __OME__tmpLoop_41_i=0;__OME__tmpLoop_41_i<2;++__OME__tmpLoop_41_i)
{

for(size_t __OME__tmpLoop_42_i=0;__OME__tmpLoop_42_i<11;++__OME__tmpLoop_42_i)
{

__OME__sum_20[__OME__tmpLoop_41_i][__OME__tmpLoop_42_i]+=(*root.GetValueArchive())[node03961.GetModelIndex()+__OME__tmpLoop_32_i+10*(node03644_i)]*(*root.GetValueArchive())[node03755.GetModelIndex()+__OME__tmpLoop_42_i+11*(__OME__tmpLoop_41_i+2*(__OME__tmpLoop_32_i))];
__OME__sum_23[__OME__tmpLoop_41_i][__OME__tmpLoop_42_i]+=(*root.GetValueArchive())[node03961.GetModelIndex()+__OME__tmpLoop_32_i+10*(node03644_i)]*(*root.GetValueArchive())[node03751.GetModelIndex()+__OME__tmpLoop_42_i+11*(__OME__tmpLoop_41_i+2*(__OME__tmpLoop_32_i))];
}
}
}
for(size_t __OME__tmpLoop_35_i=0;__OME__tmpLoop_35_i<224;++__OME__tmpLoop_35_i)
{

for(size_t __OME__tmpLoop_36_i=0;__OME__tmpLoop_36_i<11;++__OME__tmpLoop_36_i)
{

__OME__sum_16[__OME__tmpLoop_36_i]+=(*root.GetValueArchive())[node04019.GetModelIndex()+__OME__tmpLoop_35_i+224*(node03644_i)]*__OME__sum_15[__OME__tmpLoop_35_i][__OME__tmpLoop_36_i];
}
}
for(size_t __OME__tmpLoop_43_i=0;__OME__tmpLoop_43_i<2;++__OME__tmpLoop_43_i)
{

for(size_t __OME__tmpLoop_44_i=0;__OME__tmpLoop_44_i<11;++__OME__tmpLoop_44_i)
{

__OME__sum_21[__OME__tmpLoop_44_i]+=(*root.GetValueArchive())[node04020.GetModelIndex()+__OME__tmpLoop_43_i+2*(node03644_i)]*__OME__sum_20[__OME__tmpLoop_43_i][__OME__tmpLoop_44_i];
__OME__sum_24[__OME__tmpLoop_44_i]+=(*root.GetValueArchive())[node04021.GetModelIndex()+__OME__tmpLoop_43_i+2*(node03644_i)]*__OME__sum_23[__OME__tmpLoop_43_i][__OME__tmpLoop_44_i];
}
}
OME_SCALAR __OME__sum_25;
__OME__sum_25= 0.0;
OME_SCALAR __OME__sum_22;
__OME__sum_22= 0.0;
OME_SCALAR __OME__sum_19;
__OME__sum_19= 0.0;
OME_SCALAR __OME__sum_17;
__OME__sum_17= 0.0;
for(size_t __OME__tmpLoop_50_i=0;__OME__tmpLoop_50_i<11;++__OME__tmpLoop_50_i)
{

__OME__sum_17+=__OME__sum_16[__OME__tmpLoop_50_i];
__OME__sum_19+=element((OME_SCALAR*)__OME__sum_18,__OME__sum_18_dims,node04024,2)[__OME__tmpLoop_50_i];
__OME__sum_22+=__OME__sum_21[__OME__tmpLoop_50_i];
__OME__sum_25+=__OME__sum_24[__OME__tmpLoop_50_i];
}
//=-=-=-=-=-=-=-=-=-= node03963 (Est_Morbidity)
node03963.Reset(TestModelBm);
node03963=(OME_SCALAR)0.05*__OME__sum_17+__OME__sum_19+__OME__sum_22+__OME__sum_25;
node03963.BatchInitialize(TestModelTime,TestModelBm);

OME_SCALAR __OME__sum_26;
__OME__sum_26= 0.0;
__OME__sum_26+=__OME__sum_19;
//=-=-=-=-=-=-=-=-=-= node00079 (Mortality_rt)
node00079.Reset(TestModelBm);
node00079=(OME_SCALAR)__OME__sum_17+__OME__sum_26+__OME__sum_22+__OME__sum_25;
node00079.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= arc04360 (Montly_Sum)
arc04360.Reset(TestModelBm);
arc04360=(OME_SCALAR)(__OME__sum_11==1.0 ? (OME_SCALAR)node03613 : (OME_SCALAR)0.0);
arc04360.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node00230 (DU_Evap_contribution)
node00230.Reset(TestModelBm);
node00230=(OME_SCALAR)node03970/node00228*arc04360;
node00230.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node00077 (Num_Mortality)
node00077.Reset(TestModelBm);
node00077=(OME_SCALAR)node00034*node00062;
node00077.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03929 (DU_Evap)
node03929.Reset(TestModelBm);
node03929=(OME_SCALAR)__OME__sum_8;
node03929.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= arc04227 (Add_average_daily_evap)
arc04227.Reset(TestModelBm);
arc04227=(OME_SCALAR)node03929*3.401568;
arc04227.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03971 (dominant_lu)
node03971.Reset(TestModelBm);
node03971=(OME_SCALAR)__OME__posgreatest_0;
node03971.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03934 (Tree_Cover)
node03934.Reset(TestModelBm);
node03934=(OME_SCALAR)__OME__sum_9/node03970;
node03934.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03958 (NDVI)
node03958.Reset(TestModelBm);
node03958=(OME_SCALAR)__OME__sum_10;
node03958.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03979 (Av_Can_T)
node03979.Reset(TestModelBm);
node03979=(OME_SCALAR)__OME__sum_12;
node03979.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04000 (Population_modifier)
node04000.Reset(TestModelBm);
node04000=(OME_SCALAR)node03927/node03602;
node04000.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03950 (EMS)
node03950.Reset(TestModelBm);
node03950=(OME_SCALAR)OMEMax((0.0),(-8.262408+0.3614094*node03979))*node04000;
node03950.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03952 (Complaints)
node03952.Reset(TestModelBm);
node03952=(OME_SCALAR)OMEMax((0.0),(-8.031008+0.5187364*node03979))*node04000;
node03952.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03977 (Forest_T)
node03977.Reset(TestModelBm);
node03977=(OME_SCALAR)element(node04054,7.0,1);
node03977.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03990 (Bare_Ground_T)
node03990.Reset(TestModelBm);
node03990=(OME_SCALAR)element(node04054,6.0,1);
node03990.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03983 (Land_Cover)
node03983.Reset(TestModelBm);
node03983=(OME_SCALAR)element(node04060,node03637,1);
node03983.BatchInitialize(TestModelTime,TestModelBm);

}
node03644.SetCurrentInstance(0);

//=-=-=-=-=-=-=-=-=-= node03669 (month)
node03669.Reset(TestModelBm);
node03669=(OME_SCALAR)__OME__firsttrue_0;
node03669.BatchInitialize(TestModelTime,TestModelBm);

for(size_t node03700_i=0;node03700_i<1;++node03700_i)
{
node03700.SetCurrentInstance(node03700_i);

//=-=-=-=-=-=-=-=-=-= node04105 (Start_of_GCC)
node04105.BatchInitialize(TestModelTime,TestModelBm);
node04105.EvalWithVal(node03622>27.0);

node03700.GetCurrentInstanceRef()->ApplyMortality();
if (node03700.GetCurrentInstanceActive())
{
//=-=-=-=-=-=-=-=-=-= node04108 (Year)
node04108.Reset(TestModelBm);
node04108=(OME_SCALAR)node03622-27.0;
node04108.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04112 (GCC_M_Relative_Humidity)
node04112.Reset(TestModelBm);
node04112=(OME_SCALAR)element(elementRaw(node03766,node03669,2),node04108);
node04112.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04093 (GCC_M_Temp)
node04093.Reset(TestModelBm);
node04093=(OME_SCALAR)element(elementRaw(node03767,node03669,2),node04108)-273.15;
node04093.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04094 (Precip_sc)
node04094.Reset(TestModelBm);
node04094=(OME_SCALAR)element(elementRaw(node03765,node03669,2),node04108);
node04094.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04113 (GCC_Wind_speeds)
node04113.Reset(TestModelBm);
node04113=(OME_SCALAR)element(elementRaw(node03768,node03669,2),node04108);
node04113.BatchInitialize(TestModelTime,TestModelBm);

} //node03700
}
node03700.SetCurrentInstance(0);

OME_SCALAR __OME__sum_29;
__OME__sum_29= 0.0;
OME_SCALAR __OME__sum_28;
__OME__sum_28= 0.0;
OME_SCALAR __OME__sum_27;
__OME__sum_27= 0.0;
for(size_t __OME__tmpLoop_54_i=0;__OME__tmpLoop_54_i<696;++__OME__tmpLoop_54_i)
{

for(size_t __OME__tmpLoop_58_i=0;__OME__tmpLoop_58_i<15;++__OME__tmpLoop_58_i)
{

__OME__sum_30[__OME__tmpLoop_58_i]+=(*root.GetValueArchive())[node04046.GetModelIndex()+__OME__tmpLoop_58_i+15*(__OME__tmpLoop_54_i)];
__OME__sum_31[__OME__tmpLoop_58_i]+=(*root.GetValueArchive())[node04054.GetModelIndex()+__OME__tmpLoop_58_i+15*(__OME__tmpLoop_54_i)];
__OME__sum_32[__OME__tmpLoop_58_i]+=(*root.GetValueArchive())[node04052.GetModelIndex()+__OME__tmpLoop_58_i+15*(__OME__tmpLoop_54_i)];
}
__OME__sum_27+=(*root.GetValueArchive())[node03975.GetModelIndex()+__OME__tmpLoop_54_i]*(*root.GetValueArchive())[node03970.GetModelIndex()+__OME__tmpLoop_54_i];
__OME__sum_28+=(*root.GetValueArchive())[node03977.GetModelIndex()+__OME__tmpLoop_54_i]*(*root.GetValueArchive())[node03970.GetModelIndex()+__OME__tmpLoop_54_i];
__OME__sum_29+=(*root.GetValueArchive())[node03990.GetModelIndex()+__OME__tmpLoop_54_i]*(*root.GetValueArchive())[node03970.GetModelIndex()+__OME__tmpLoop_54_i];
}
for(size_t __OME__tmpLoop_59_i=0;__OME__tmpLoop_59_i<15;++__OME__tmpLoop_59_i)
{

//=-=-=-=-=-=-=-=-=-= node03677 (LandUse_air_T)
(*root.GetValueArchive())[node03677.GetModelIndex()+__OME__tmpLoop_59_i]=(OME_SCALAR)__OME__sum_30[__OME__tmpLoop_59_i]/696.0;
node03677.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03696 (LandUse_Canopy_T)
(*root.GetValueArchive())[node03696.GetModelIndex()+__OME__tmpLoop_59_i]=(OME_SCALAR)__OME__sum_31[__OME__tmpLoop_59_i]/696.0;
node03696.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03728 (Temp_red)
(*root.GetValueArchive())[node03728.GetModelIndex()+__OME__tmpLoop_59_i]=(OME_SCALAR)__OME__sum_32[__OME__tmpLoop_59_i]/696.0;
node03728.BatchInitialize(TestModelTime,TestModelBm);

}
OME_SCALAR __OME__sum_34;
__OME__sum_34= 0.0;
OME_SCALAR __OME__sum_33;
__OME__sum_33= 0.0;
for(size_t __OME__tmpLoop_66_i=0;__OME__tmpLoop_66_i<1;++__OME__tmpLoop_66_i)
{
node03717.SetCurrentInstance(__OME__tmpLoop_66_i);
node03730.SetCurrentInstance(__OME__tmpLoop_66_i);

__OME__sum_33+=(*root.GetValueArchive())[node04094.GetModelIndex()];
__OME__sum_34+=(*root.GetValueArchive())[node04113.GetModelIndex()];
OME_SCALAR __OME__firsttrue_4;
__OME__firsttrue_4=0;
OME_SCALAR __OME__sum_38;
__OME__sum_38= 0.0;
node03717.GetCurrentInstanceRef()->ApplyMortality();
if (node03717.GetCurrentInstanceActive())
{
for(size_t __OME__tmpLoop_72_i=0;__OME__tmpLoop_72_i<696;++__OME__tmpLoop_72_i)
{

__OME__sum_38+=(*root.GetValueArchive())[node00230.GetModelIndex()+__OME__tmpLoop_72_i];
}
OME_SCALAR __OME__tempArray_19[]={31.0,59.0,90.0,120.0,151.0,181.0,212.0,243.0,273.0,304.0,334.0,365.0};
size_t __OME__tempArray_19_dims[]={12};
for(size_t __OME__tmpLoop_73_i=0;__OME__tmpLoop_73_i<12;++__OME__tmpLoop_73_i)
{

if(!__OME__firsttrue_4 && node03638==__OME__tempArray_19[__OME__tmpLoop_73_i]) __OME__firsttrue_4=__OME__tmpLoop_73_i+1;
}
//=-=-=-=-=-=-=-=-=-= node04138 (month)
node04138.Reset(TestModelBm);
node04138=(OME_SCALAR)__OME__firsttrue_4;
node04138.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04135 (Measured_Values)
node04135.Reset(TestModelBm);
node04135=(OME_SCALAR)element(elementRaw(node03734,node03622,2),node04138);
node04135.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04147 (Montly_Evaporation)
node04147.Reset(TestModelBm);
node04147=(OME_SCALAR)__OME__sum_38;
node04147.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04140 (year)
node04140.Reset(TestModelBm);
node04140=(OME_SCALAR)node03622+1972.0;
node04140.BatchInitialize(TestModelTime,TestModelBm);

} //node03717
OME_SCALAR __OME__sum_40;
__OME__sum_40= 0.0;
OME_SCALAR __OME__sum_39;
__OME__sum_39= 0.0;
//=-=-=-=-=-=-=-=-=-= node04153 (cond1)
node04153.BatchInitialize(TestModelTime,TestModelBm);
node04153.EvalWithVal(node03622==39.0&&node03638>152.0&&node03638<274.0);

node03730.GetCurrentInstanceRef()->ApplyMortality();
if (node03730.GetCurrentInstanceActive())
{
for(size_t __OME__tmpLoop_74_i=0;__OME__tmpLoop_74_i<696;++__OME__tmpLoop_74_i)
{

__OME__sum_39+=(*root.GetValueArchive())[node03950.GetModelIndex()+__OME__tmpLoop_74_i];
__OME__sum_40+=(*root.GetValueArchive())[node03952.GetModelIndex()+__OME__tmpLoop_74_i];
}
//=-=-=-=-=-=-=-=-=-= node04157 (EMS_model)
node04157.Reset(TestModelBm);
node04157=(OME_SCALAR)__OME__sum_39;
node04157.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04163 (EMS_data)
node04163.Reset(TestModelBm);
node04163=(OME_SCALAR)elementRaw(node04173,node03638-152.0,1);
node04163.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04169 (EMS_sq_Err)
node04169.Reset(TestModelBm);
node04169=(OME_SCALAR)OMECFuncs::pow((node04157-node04163),2.0);
node04169.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04159 (Complaints_model)
node04159.Reset(TestModelBm);
node04159=(OME_SCALAR)__OME__sum_40;
node04159.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04165 (Complaints)
node04165.Reset(TestModelBm);
node04165=(OME_SCALAR)elementRaw(node04174,node03638-152.0,1);
node04165.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node04171 (Complaints__sq_err)
node04171.Reset(TestModelBm);
node04171=(OME_SCALAR)OMECFuncs::pow((node04165-node04159),2.0);
node04171.BatchInitialize(TestModelTime,TestModelBm);

} //node03730
node03717.GetCurrentInstanceRef()->ApplyMortality();
if (node03717.GetCurrentInstanceActive())
{
OME_SCALAR __OME__tempArray_20[]={node04135,node04147,node04140};
size_t __OME__tempArray_20_dims[]={3};
for(size_t __OME__tmpLoop_80_i=0;__OME__tmpLoop_80_i<3;++__OME__tmpLoop_80_i)
{

//=-=-=-=-=-=-=-=-=-= node04142 (Comparison)
(*root.GetValueArchive())[node04142.GetModelIndex()+__OME__tmpLoop_80_i]=(OME_SCALAR)__OME__tempArray_20[__OME__tmpLoop_80_i];
node04142.BatchInitialize(TestModelTime,TestModelBm);

}
} //node03717
}
node03717.SetCurrentInstance(0);
node03730.SetCurrentInstance(0);

//=-=-=-=-=-=-=-=-=-= node03668 (Windspeed)
node03668.Reset(TestModelBm);
node03668=(OME_SCALAR)(currenttime(TestModelBm)==0.0 ? (OME_SCALAR)0.0 : (OME_SCALAR)((node03612==0.0 ? (OME_SCALAR)element(element(node03781,node03638,2),node03622) : (OME_SCALAR)__OME__sum_34)));
node03668.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node00060 (Mortality_Choice)
node00060.BatchInitialize(TestModelTime,TestModelBm);
node00060.Evaluate(TestModelTime,TestModelBm);

for(size_t node03644_i=0;node03644_i<696;++node03644_i)
{
node03644.SetCurrentInstance(node03644_i);
node03642.SetCurrentInstance(node03644_i);

for(size_t node04002_i=0;node04002_i<6;++node04002_i)
{
node04002.SetCurrentInstance(node04002_i);

OME_SCALAR __OME__sum_35;
__OME__sum_35= 0.0;
for(size_t __OME__tmpLoop_68_i=0;__OME__tmpLoop_68_i<15;++__OME__tmpLoop_68_i)
{

__OME__sum_35+=(*root.GetValueArchive())[node04030.GetModelIndex()+__OME__tmpLoop_68_i+15*(node03644_i)];
}
//=-=-=-=-=-=-=-=-=-= node04080 (Pollutant_removal_potential)
node04080.Reset(TestModelBm);
node04080=(OME_SCALAR)__OME__sum_35*elementRaw(node03788,node04002_i+1,1);
node04080.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= arc04194 (Pollutant_removal)
arc04194.Reset(TestModelBm);
arc04194=(OME_SCALAR)OMEMax((0.0),(OMEMin((node04081),(node04080))));
arc04194.BatchInitialize(TestModelTime,TestModelBm);

}
node04002.SetCurrentInstance(0);

OME_SCALAR __OME__sum_36;
__OME__sum_36= 0.0;
for(size_t __OME__tmpLoop_69_i=0;__OME__tmpLoop_69_i<15;++__OME__tmpLoop_69_i)
{

__OME__sum_36+=(*root.GetValueArchive())[node04034.GetModelIndex()+__OME__tmpLoop_69_i+15*(node03644_i)];
}
for(size_t node03847_i=0;node03847_i<5;++node03847_i)
{
node03847.SetCurrentInstance(node03847_i);

OME_SCALAR __OME__tempArray_18[]={elementRaw(node03950,node03644_i+1,1),elementRaw(node03952,node03644_i+1,1),elementRaw(node00079,node03644_i+1,1),elementRaw(node00077,node03644_i+1,1),elementRaw(node00073,node03644_i+1,1)};
size_t __OME__tempArray_18_dims[]={5};
//=-=-=-=-=-=-=-=-=-= node03904 (Morbidity)
node03904.Reset(TestModelBm);
node03904=(OME_SCALAR)element((OME_SCALAR*)__OME__tempArray_18,__OME__tempArray_18_dims,node03847_i+1,1);
node03904.BatchInitialize(TestModelTime,TestModelBm);

}
node03847.SetCurrentInstance(0);

//=-=-=-=-=-=-=-=-=-= node04004 (Windspeed)
node04004.Reset(TestModelBm);
node04004=(OME_SCALAR)node03668-__OME__sum_36;
node04004.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03939 (Air_stagnation)
node03939.Reset(TestModelBm);
node03939=(OME_SCALAR)(node04004<13.0 ? (OME_SCALAR)1.0 : (OME_SCALAR)0.0);
node03939.BatchInitialize(TestModelTime,TestModelBm);

OME_SCALAR __OME__tempArray_17[]={node00079,node00077,node00073};
size_t __OME__tempArray_17_dims[]={3};
//=-=-=-=-=-=-=-=-=-= node00074 (Mortality)
node00074.Reset(TestModelBm);
node00074=(OME_SCALAR)element((OME_SCALAR*)__OME__tempArray_17,__OME__tempArray_17_dims,node00060,1);
node00074.BatchInitialize(TestModelTime,TestModelBm);

}
node03644.SetCurrentInstance(0);
node03642.SetCurrentInstance(0);

//=-=-=-=-=-=-=-=-=-= node03694 (Choose_Heat_Mortality_Index)
node03694.BatchInitialize(TestModelTime,TestModelBm);
node03694.Evaluate(TestModelTime,TestModelBm);

for(size_t __OME__tmpLoop_70_i=0;__OME__tmpLoop_70_i<696;++__OME__tmpLoop_70_i)
{
node03644.SetCurrentInstance(__OME__tmpLoop_70_i);
node03642.SetCurrentInstance(__OME__tmpLoop_70_i);

for(size_t __OME__tmpLoop_71_i=0;__OME__tmpLoop_71_i<5;++__OME__tmpLoop_71_i)
{
node03847.SetCurrentInstance(__OME__tmpLoop_71_i);

__OME__sum_37[__OME__tmpLoop_71_i]+=(*root.GetValueArchive())[node03904.GetModelIndex()+__OME__tmpLoop_71_i+5*(__OME__tmpLoop_70_i)];
//=-=-=-=-=-=-=-=-=-= arc03930 (add_mortalities_rt)
arc03930.Reset(TestModelBm);
arc03930=(OME_SCALAR)node03904-((node03638==365.0 ? (OME_SCALAR)node03896 : (OME_SCALAR)0.0));
arc03930.BatchInitialize(TestModelTime,TestModelBm);

}
node03847.SetCurrentInstance(0);

for(size_t node04002_i=0;node04002_i<6;++node04002_i)
{
node04002.SetCurrentInstance(node04002_i);

//=-=-=-=-=-=-=-=-=-= node04071 (diffusion_rt)
node04071.Reset(TestModelBm);
node04071=(OME_SCALAR)(elementRaw(node03794,node04002_i+1,1)==1.0 ? (OME_SCALAR)(1.0-node03939)*elementRaw(node03800,node04002_i+1,1) : (OME_SCALAR)elementRaw(node03800,node04002_i+1,1));
node04071.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= arc04201 (Pollutant_production)
arc04201.Reset(TestModelBm);
arc04201=(OME_SCALAR)(elementRaw(node03797,node04002_i+1,1)-node04081)*node04071;
arc04201.BatchInitialize(TestModelTime,TestModelBm);

}
node04002.SetCurrentInstance(0);

OME_SCALAR __OME__sum_41;
__OME__sum_41= 0.0;
for(size_t __OME__tmpLoop_76_i=0;__OME__tmpLoop_76_i<696;++__OME__tmpLoop_76_i)
{

__OME__sum_41+=(*root.GetValueArchive())[node03975.GetModelIndex()+__OME__tmpLoop_76_i];
}
//=-=-=-=-=-=-=-=-=-= node03869 (Air_temperatures_deviation)
node03869.Reset(TestModelBm);
node03869=(OME_SCALAR)elementRaw(node03975,__OME__tmpLoop_70_i+1,1)-__OME__sum_41/node03644.GetNumInstances();
node03869.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03893 (Bare_ground_T)
node03893.Reset(TestModelBm);
node03893=(OME_SCALAR)elementRaw(node03990,__OME__tmpLoop_70_i+1,1);
node03893.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03883 (Landcover)
node03883.Reset(TestModelBm);
node03883=(OME_SCALAR)elementRaw(node03983,__OME__tmpLoop_70_i+1,1);
node03883.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03880 (Surface_Temperature)
node03880.Reset(TestModelBm);
node03880=(OME_SCALAR)elementRaw(node03979,__OME__tmpLoop_70_i+1,1);
node03880.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03877 (Dominant_LU)
node03877.Reset(TestModelBm);
node03877=(OME_SCALAR)elementRaw(node03971,__OME__tmpLoop_70_i+1,1);
node03877.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03871 (Heat_Wave)
node03871.Reset(TestModelBm);
node03871=(OME_SCALAR)elementRaw(node03965,__OME__tmpLoop_70_i+1,1);
node03871.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03841 (Water_temperature_reduction)
node03841.Reset(TestModelBm);
node03841=(OME_SCALAR)element(elementRaw(node04052,__OME__tmpLoop_70_i+1,2),1.0);
node03841.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03851 (Morbidity_est)
node03851.Reset(TestModelBm);
node03851=(OME_SCALAR)element(node03904,node03694,1);
node03851.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03856 (Tree_cover_ratio)
node03856.Reset(TestModelBm);
node03856=(OME_SCALAR)elementRaw(node03934,__OME__tmpLoop_70_i+1,1);
node03856.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03863 (NDVI)
node03863.Reset(TestModelBm);
node03863=(OME_SCALAR)elementRaw(node03958,__OME__tmpLoop_70_i+1,1);
node03863.BatchInitialize(TestModelTime,TestModelBm);

}
node03644.SetCurrentInstance(0);
node03642.SetCurrentInstance(0);

for(size_t node03611_i=0;node03611_i<6;++node03611_i)
{
node03611.SetCurrentInstance(node03611_i);

OME_SCALAR __OME__sum_42[6];
size_t __OME__sum_42_dims[]={6};
for(size_t __OME__iniLoop_17_i=0;__OME__iniLoop_17_i<6;++__OME__iniLoop_17_i)
{

__OME__sum_42[__OME__iniLoop_17_i]= 0.0;
}
for(size_t __OME__tmpLoop_77_i=0;__OME__tmpLoop_77_i<696;++__OME__tmpLoop_77_i)
{

for(size_t __OME__tmpLoop_78_i=0;__OME__tmpLoop_78_i<6;++__OME__tmpLoop_78_i)
{

__OME__sum_42[__OME__tmpLoop_78_i]+=(*root.GetValueArchive())[arc04194.GetModelIndex()+__OME__tmpLoop_78_i+6*(__OME__tmpLoop_77_i)];
}
}
OME_SCALAR __OME__tmp_lst_0[6]={};
size_t __OME__tmp_lst_0_dims[]={6};
for(size_t __OME__tmpLoop_79_i=0;__OME__tmpLoop_79_i<6;++__OME__tmpLoop_79_i)
{

__OME__tmp_lst_0[__OME__tmpLoop_79_i]=__OME__sum_42[__OME__tmpLoop_79_i]*node03790/2000.0;
}
//=-=-=-=-=-=-=-=-=-= node03792 (Total_pollutant_Removal_Value)
node03792.Reset(TestModelBm);
node03792=(OME_SCALAR)element((OME_SCALAR*)__OME__tmp_lst_0,__OME__tmp_lst_0_dims,node03611_i+1,1);
node03792.BatchInitialize(TestModelTime,TestModelBm);

}
node03611.SetCurrentInstance(0);

//=-=-=-=-=-=-=-=-=-= node03720 (EMS_modeled)
node03720.Reset(TestModelBm);
node03720=(OME_SCALAR)element((OME_SCALAR*)__OME__sum_37,__OME__sum_37_dims,1.0,1);
node03720.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03723 (EMS_cat)
node03723.Reset(TestModelBm);
node03723=(OME_SCALAR)element((OME_SCALAR*)__OME__sum_37,__OME__sum_37_dims,3.0,1);
node03723.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03725 (EMS_num)
node03725.Reset(TestModelBm);
node03725=(OME_SCALAR)element((OME_SCALAR*)__OME__sum_37,__OME__sum_37_dims,2.0,1);
node03725.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03704 (Chance_Position)
node03704.Reset(TestModelBm);
node03704=(OME_SCALAR)__OME__firsttrue_2;
node03704.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03706 (Ratio_of_rain_by_month)
node03706.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03693 (GCC_Rainfall)
node03693.Reset(TestModelBm);
node03693=(OME_SCALAR)__OME__sum_33*element(element(node03706,node03704,2),node03669)*3.0;
node03693.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03707 (Rain)
node03707.Reset(TestModelBm);
node03707=(OME_SCALAR)(currenttime(TestModelBm)==0.0 ? (OME_SCALAR)0.0 : (OME_SCALAR)((node03612==0.0 ? (OME_SCALAR)element(element(node03786,node03638,2),node03622) : (OME_SCALAR)node03693)));
node03707.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= arc04266 (add_precip)
arc04266.Reset(TestModelBm);
arc04266=(OME_SCALAR)node03707;
arc04266.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= arc04268 (remove_precip)
arc04268.Reset(TestModelBm);
arc04268=(OME_SCALAR)lookupInit_var_delay(TestModelBm,(arc04266),((currenttime(TestModelBm)<node03682 ? (OME_SCALAR)1.0 : (OME_SCALAR)node03682)));
arc04268.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03603 (Avr_T)
node03603.Reset(TestModelBm);
node03603=(OME_SCALAR)__OME__sum_27/__OME__sum_0;
node03603.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03617 (Forest__Temperatures)
node03617.Reset(TestModelBm);
node03617=(OME_SCALAR)__OME__sum_28/__OME__sum_0;
node03617.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03619 (Bare_ground_Temperatures)
node03619.Reset(TestModelBm);
node03619=(OME_SCALAR)__OME__sum_29/__OME__sum_0;
node03619.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= arc04234 (Add_RH)
arc04234.Reset(TestModelBm);
arc04234=(OME_SCALAR)node03620;
arc04234.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= arc04232 (remove_RH)
arc04232.Reset(TestModelBm);
arc04232=(OME_SCALAR)lookupInit_var_delay(TestModelBm,(arc04234),((currenttime(TestModelBm)<node03682 ? (OME_SCALAR)1.0 : (OME_SCALAR)node03682)));
arc04232.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= arc04241 (add_T)
arc04241.Reset(TestModelBm);
arc04241=(OME_SCALAR)node03604;
arc04241.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= arc04229 (remove_T)
arc04229.Reset(TestModelBm);
arc04229=(OME_SCALAR)lookupInit_var_delay(TestModelBm,(arc04241),((currenttime(TestModelBm)<node03682 ? (OME_SCALAR)1.0 : (OME_SCALAR)node03682)));
arc04229.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03656 (RH_St_Dev)
node03656.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03657 (Temp_C_St_Dev)
node03657.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= arc03874 (plus_year)
arc03874.Reset(TestModelBm);
arc03874=(OME_SCALAR)(currenttime(TestModelBm)==0.0||node03638==365.0 ? (OME_SCALAR)1.0 : (OME_SCALAR)0.0);
arc03874.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03640 (Regional_Threshold_T)
node03640.Reset(TestModelBm);
node03640=(OME_SCALAR)28.28;
node03640.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03648 (HeatIndex)
node03648.BatchInitialize(TestModelTime,TestModelBm);
node03648.Evaluate(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03718 (Heat_Index_in_C)
node03718.Reset(TestModelBm);
node03718=(OME_SCALAR)node03648*5.0/9.0-32.0;
node03718.BatchInitialize(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03722 (EMS_measured)
node03722.BatchInitialize(TestModelTime,TestModelBm);
node03722.Evaluate(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03687 (P)
node03687.Reset(TestModelBm);
node03687=(OME_SCALAR)0.0;
node03687.BatchInitialize(TestModelTime,TestModelBm);
AdjustTempPool();
}

void TestModel::DoUpdate(const OME_SCALAR & TestModelTime, BaseManager* TestModelBm,ISpatialDataProvider* TestModelSDP)
{
using namespace TestModelDefs;
using namespace OMECFuncs;
Model& root=*gp_root;
Variable& node00059=*gp_node00059;
Variable& node00062=*gp_node00062;
Variable& node00228=*gp_node00228;
Variable& node03602=*gp_node03602;
Variable& node03603=*gp_node03603;
Variable& node03604=*gp_node03604;
Variable& node03605=*gp_node03605;
Variable& node03607=*gp_node03607;
Variable& node03612=*gp_node03612;
Variable& node03614=*gp_node03614;
Variable& node03617=*gp_node03617;
Variable& node03619=*gp_node03619;
Variable& node03620=*gp_node03620;
Variable& node03626=*gp_node03626;
Variable& node03628=*gp_node03628;
Variable& node03630=*gp_node03630;
Variable& node03632=*gp_node03632;
Variable& node03634=*gp_node03634;
Variable& node03638=*gp_node03638;
Variable& node03640=*gp_node03640;
Variable& node03651=*gp_node03651;
Variable& node03653=*gp_node03653;
Variable& node03668=*gp_node03668;
Variable& node03669=*gp_node03669;
Variable& node03671=*gp_node03671;
Variable& node03672=*gp_node03672;
Variable& node03680=*gp_node03680;
Variable& node03682=*gp_node03682;
Variable& node03683=*gp_node03683;
Variable& node03685=*gp_node03685;
Variable& node03693=*gp_node03693;
Variable& node03695=*gp_node03695;
Variable& node03697=*gp_node03697;
Variable& node03702=*gp_node03702;
Variable& node03704=*gp_node03704;
Variable& node03707=*gp_node03707;
Variable& node03709=*gp_node03709;
Variable& node03712=*gp_node03712;
Variable& node03718=*gp_node03718;
Variable& node03720=*gp_node03720;
Variable& node03723=*gp_node03723;
Variable& node03725=*gp_node03725;
VarArray& node03594=*gp_node03594;
VarArray& node03596=*gp_node03596;
VarArray& node03598=*gp_node03598;
VarArray& node03600=*gp_node03600;
VarArray& node03641=*gp_node03641;
VarArray& node03656=*gp_node03656;
VarArray& node03657=*gp_node03657;
VarArray& node03674=*gp_node03674;
VarArray& node03677=*gp_node03677;
VarArray& node03696=*gp_node03696;
VarArray& node03699=*gp_node03699;
VarArray& node03706=*gp_node03706;
VarArray& node03728=*gp_node03728;
TimeSeriesVar& node00060=*gp_node00060;
TimeSeriesVar& node03637=*gp_node03637;
TimeSeriesVar& node03648=*gp_node03648;
TimeSeriesVar& node03694=*gp_node03694;
TimeSeriesVar& node03722=*gp_node03722;
StateVar& node03622=*gp_node03622;
StateVar& node03646=*gp_node03646;
StateVar& node03663=*gp_node03663;
StateVar& node03687=*gp_node03687;
Flow& arc03874=*gp_arc03874;
Flow& arc04241=*gp_arc04241;
Flow& arc04229=*gp_arc04229;
Flow& arc04232=*gp_arc04232;
Flow& arc04234=*gp_arc04234;
Flow& arc04266=*gp_arc04266;
Flow& arc04268=*gp_arc04268;
Model& node03593=*gp_node03593;
Model& node03732=*gp_node03732;
Variable& node03734=*gp_node03734;
Model& node03595=*gp_node03595;
Model& node03739=*gp_node03739;
Model& node03744=*gp_node03744;
Variable& node03746=*gp_node03746;
Model& node03740=*gp_node03740;
Model& node03749=*gp_node03749;
Variable& node03751=*gp_node03751;
Model& node03741=*gp_node03741;
Model& node03753=*gp_node03753;
Variable& node03755=*gp_node03755;
Model& node03743=*gp_node03743;
Model& node03756=*gp_node03756;
Variable& node03759=*gp_node03759;
Model& node03606=*gp_node03606;
Model& node03760=*gp_node03760;
Variable& node03765=*gp_node03765;
Variable& node03766=*gp_node03766;
Variable& node03767=*gp_node03767;
Variable& node03768=*gp_node03768;
Model& node03608=*gp_node03608;
Model& node03776=*gp_node03776;
Variable& node03781=*gp_node03781;
Variable& node03783=*gp_node03783;
Variable& node03784=*gp_node03784;
Variable& node03786=*gp_node03786;
Model& node03611=*gp_node03611;
Variable& node03788=*gp_node03788;
Variable& node03790=*gp_node03790;
Variable& node03792=*gp_node03792;
Variable& node03794=*gp_node03794;
Variable& node03797=*gp_node03797;
Variable& node03800=*gp_node03800;
Model& node03627=*gp_node03627;
Variable& node03804=*gp_node03804;
Variable& node03809=*gp_node03809;
Variable& node03812=*gp_node03812;
Variable& node03814=*gp_node03814;
Variable& node03817=*gp_node03817;
Variable& node03820=*gp_node03820;
Variable& node03821=*gp_node03821;
Variable& node03823=*gp_node03823;
Variable& node03973=*gp_node03973;
Model& node03642=*gp_node03642;
Variable& node03825=*gp_node03825;
Variable& node03826=*gp_node03826;
Variable& node03828=*gp_node03828;
Variable& node03830=*gp_node03830;
Variable& node03834=*gp_node03834;
Variable& node03836=*gp_node03836;
Variable& node03841=*gp_node03841;
Variable& node03844=*gp_node03844;
Variable& node03848=*gp_node03848;
Variable& node03851=*gp_node03851;
Variable& node03853=*gp_node03853;
Variable& node03856=*gp_node03856;
Variable& node03862=*gp_node03862;
Variable& node03863=*gp_node03863;
Variable& node03869=*gp_node03869;
Variable& node03871=*gp_node03871;
Variable& node03872=*gp_node03872;
Variable& node03877=*gp_node03877;
Variable& node03880=*gp_node03880;
Variable& node03883=*gp_node03883;
Variable& node03893=*gp_node03893;
Model& node03847=*gp_node03847;
Variable& node03904=*gp_node03904;
StateVar& node03896=*gp_node03896;
Flow& arc03930=*gp_arc03930;
Model& node03644=*gp_node03644;
Variable& node00073=*gp_node00073;
Variable& node00074=*gp_node00074;
Variable& node00077=*gp_node00077;
Variable& node00079=*gp_node00079;
Variable& node00230=*gp_node00230;
Variable& node03927=*gp_node03927;
Variable& node03929=*gp_node03929;
Variable& node03934=*gp_node03934;
Variable& node03939=*gp_node03939;
Variable& node03950=*gp_node03950;
Variable& node03952=*gp_node03952;
Variable& node03954=*gp_node03954;
Variable& node03958=*gp_node03958;
Variable& node03963=*gp_node03963;
Variable& node03965=*gp_node03965;
Variable& node03970=*gp_node03970;
Variable& node03971=*gp_node03971;
Variable& node03975=*gp_node03975;
Variable& node03977=*gp_node03977;
Variable& node03979=*gp_node03979;
Variable& node03983=*gp_node03983;
Variable& node03990=*gp_node03990;
Variable& node04000=*gp_node04000;
Variable& node04004=*gp_node04004;
VarArray& node03961=*gp_node03961;
StateVar& node03613=*gp_node03613;
Flow& arc04227=*gp_arc04227;
Flow& arc04360=*gp_arc04360;
Model& node00017=*gp_node00017;
Variable& node00021=*gp_node00021;
Variable& node00034=*gp_node00034;
Variable& node00036=*gp_node00036;
Variable& node00045=*gp_node00045;
Variable& node00046=*gp_node00046;
Variable& node00047=*gp_node00047;
Variable& node00048=*gp_node00048;
StateVar& node00024=*gp_node00024;
StateVar& node00030=*gp_node00030;
StateVar& node00052=*gp_node00052;
Flow& arc01510=*gp_arc01510;
Flow& arc01203=*gp_arc01203;
Flow& arc01406=*gp_arc01406;
Flow& arc01511=*gp_arc01511;
Flow& arc01738=*gp_arc01738;
Flow& arc01739=*gp_arc01739;
Model& node03967=*gp_node03967;
Variable& node04013=*gp_node04013;
Variable& node04015=*gp_node04015;
Variable& node04023=*gp_node04023;
Variable& node04024=*gp_node04024;
VarArray& node04019=*gp_node04019;
VarArray& node04020=*gp_node04020;
VarArray& node04021=*gp_node04021;
Model& node03988=*gp_node03988;
Variable& node04030=*gp_node04030;
Variable& node04034=*gp_node04034;
Variable& node04039=*gp_node04039;
Variable& node04041=*gp_node04041;
Variable& node04046=*gp_node04046;
Variable& node04052=*gp_node04052;
Variable& node04054=*gp_node04054;
Variable& node04056=*gp_node04056;
Variable& node04060=*gp_node04060;
Variable& node04064=*gp_node04064;
StateVar& node04043=*gp_node04043;
Model& node04002=*gp_node04002;
Variable& node04071=*gp_node04071;
Variable& node04080=*gp_node04080;
StateVar& node04081=*gp_node04081;
Flow& arc04201=*gp_arc04201;
Flow& arc04194=*gp_arc04194;
Model& node03655=*gp_node03655;
Model& node04090=*gp_node04090;
VarArray& node04091=*gp_node04091;
Model& node03700=*gp_node03700;
Variable& node04093=*gp_node04093;
Variable& node04094=*gp_node04094;
Variable& node04108=*gp_node04108;
Variable& node04112=*gp_node04112;
Variable& node04113=*gp_node04113;
Spawner& node04105=*gp_node04105;
Model& node03711=*gp_node03711;
Variable& node04115=*gp_node04115;
Variable& node04117=*gp_node04117;
Variable& node04118=*gp_node04118;
Variable& node04119=*gp_node04119;
Variable& node04121=*gp_node04121;
Variable& node04122=*gp_node04122;
Variable& node04123=*gp_node04123;
Variable& node04124=*gp_node04124;
Variable& node04130=*gp_node04130;
Model& node03717=*gp_node03717;
Variable& node04135=*gp_node04135;
Variable& node04138=*gp_node04138;
Variable& node04140=*gp_node04140;
Variable& node04147=*gp_node04147;
Variable& node04150=*gp_node04150;
VarArray& node04142=*gp_node04142;
Spawner& node04144=*gp_node04144;
Model& node03730=*gp_node03730;
Variable& node04157=*gp_node04157;
Variable& node04159=*gp_node04159;
Variable& node04163=*gp_node04163;
Variable& node04165=*gp_node04165;
Variable& node04169=*gp_node04169;
Variable& node04171=*gp_node04171;
Spawner& node04153=*gp_node04153;
Model& node03731=*gp_node03731;
Variable& node04173=*gp_node04173;
Variable& node04174=*gp_node04174;

OME_SCALAR __OME__sum_37[5];
size_t __OME__sum_37_dims[]={5};
OME_SCALAR __OME__sum_1[5];
size_t __OME__sum_1_dims[]={5};
for(size_t __OME__iniLoop_0_i=0;__OME__iniLoop_0_i<5;++__OME__iniLoop_0_i)
{

__OME__sum_37[__OME__iniLoop_0_i]= 0.0;
__OME__sum_1[__OME__iniLoop_0_i]= 0.0;
}
OME_SCALAR __OME__sum_31[15];
size_t __OME__sum_31_dims[]={15};
OME_SCALAR __OME__sum_30[15];
size_t __OME__sum_30_dims[]={15};
OME_SCALAR __OME__sum_29[15];
size_t __OME__sum_29_dims[]={15};

OME_SCALAR __OME__sum_3;
__OME__sum_3= 0.0;
OME_SCALAR __OME__sum_2;
__OME__sum_2= 0.0;
for(size_t node03644_i=0;node03644_i<696;++node03644_i)
{
node03644.SetCurrentInstance(node03644_i);

OME_SCALAR __OME__sum_0;
__OME__sum_0= 0.0;
for(size_t __OME__tmpLoop_0_i=0;__OME__tmpLoop_0_i<6;++__OME__tmpLoop_0_i)
{

__OME__sum_0+=(*root.GetValueArchive())[node04081.GetModelIndex()+__OME__tmpLoop_0_i+6*(node03644_i)];
}

OME_SCALAR __OME__tempArray_8[]={1.0-node04013,node04013};
size_t __OME__tempArray_8_dims[]={2};
OME_SCALAR __OME__tempArray_9[]={node04015,1.0-node04015};
size_t __OME__tempArray_9_dims[]={2};
for(size_t __OME__tmpLoop_14_i=0;__OME__tmpLoop_14_i<2;++__OME__tmpLoop_14_i)
{

//=-=-=-=-=-=-=-=-=-= node04020 (Life_Style)
(*root.GetValueArchive())[node04020.GetModelIndex()+__OME__tmpLoop_14_i+2*(node03644_i)]=(OME_SCALAR)__OME__tempArray_8[__OME__tmpLoop_14_i];

//=-=-=-=-=-=-=-=-=-= node04021 (Employment)
(*root.GetValueArchive())[node04021.GetModelIndex()+__OME__tmpLoop_14_i+2*(node03644_i)]=(OME_SCALAR)__OME__tempArray_9[__OME__tmpLoop_14_i];

}
//=-=-=-=-=-=-=-=-=-= node00046 (Intensity)
node00046=(OME_SCALAR)(node00030==0.0 ? (OME_SCALAR)0.0 : (OME_SCALAR)node00024/node00030);

//=-=-=-=-=-=-=-=-=-= node00048 (heatwave)
node00048=(OME_SCALAR)(node00030>=2.0 ? (OME_SCALAR)1.0 : (OME_SCALAR)0.0);

//=-=-=-=-=-=-=-=-=-= node00036 (A_and_B_HW)
node00036=(OME_SCALAR)node00048*OMEMax((node00030),(2.0))+node00052;


for(size_t __OME__tmpLoop_3_i=0;__OME__tmpLoop_3_i<5;++__OME__tmpLoop_3_i)
{

__OME__sum_1[__OME__tmpLoop_3_i]+=(*root.GetValueArchive())[node03896.GetModelIndex()+__OME__tmpLoop_3_i+5*(node03644_i)];
}
//=-=-=-=-=-=-=-=-=-= node03954 (T_Air_pollution)
node03954=(OME_SCALAR)__OME__sum_0;

__OME__sum_2+=(*root.GetValueArchive())[node03970.GetModelIndex()+node03644_i];
__OME__sum_3+=(*root.GetValueArchive())[node03927.GetModelIndex()+node03644_i];
}
node03644.SetCurrentInstance(0);

//=-=-=-=-=-=-=-=-=-= node03638 (JDAY)
node03638=(OME_SCALAR)::fmod((currenttime(TestModelBm)),(365.0))+1.0;

for(size_t node03717_i=0;node03717_i<1;++node03717_i)
{
node03717.SetCurrentInstance(node03717_i);

OME_SCALAR __OME__any_0;
OME_SCALAR __OME__tempArray_0[]={31.0,59.0,90.0,120.0,151.0,181.0,212.0,243.0,273.0,304.0,334.0,365.0};
size_t __OME__tempArray_0_dims[]={12};
__OME__any_0=0;
for(size_t __OME__tmpLoop_1_i=0;__OME__tmpLoop_1_i<12;++__OME__tmpLoop_1_i)
{

__OME__any_0=__OME__any_0 || node03638==__OME__tempArray_0[__OME__tmpLoop_1_i];
}
//=-=-=-=-=-=-=-=-=-= node04144 (End_of_the_month)
node04144.EvalWithVal(node03622<=39.0&&__OME__any_0);

node03717.GetCurrentInstanceRef()->ApplyMortality();
if (node03717.GetCurrentInstanceActive())
{
//=-=-=-=-=-=-=-=-=-= node04150 (retrieve_sum)
node04150=(OME_SCALAR)1.0;

} //node03717
}
node03717.SetCurrentInstance(0);

for(size_t __OME__tmpLoop_4_i=0;__OME__tmpLoop_4_i<5;++__OME__tmpLoop_4_i)
{

//=-=-=-=-=-=-=-=-=-= node03594 (Total_Mortalities)
(*root.GetValueArchive())[node03594.GetModelIndex()+__OME__tmpLoop_4_i]=(OME_SCALAR)__OME__sum_1[__OME__tmpLoop_4_i];

}
OME_SCALAR __OME__tempArray_1[]={33.0,17.0,1.0,34.0,18.0,2.0,35.0,19.0,3.0,36.0,20.0,4.0,37.0,21.0,5.0,38.0,22.0,6.0,39.0,23.0,7.0,40.0,24.0,8.0,41.0,25.0,9.0,42.0,26.0,10.0,43.0,27.0,11.0,44.0,28.0,12.0,45.0,29.0,13.0,46.0,30.0,14.0,47.0,31.0,15.0,48.0,32.0,16.0};
size_t __OME__tempArray_1_dims[]={48};
for(size_t __OME__tmpLoop_7_i=0;__OME__tmpLoop_7_i<48;++__OME__tmpLoop_7_i)
{

//=-=-=-=-=-=-=-=-=-= node03596 (Children)
(*root.GetValueArchive())[node03596.GetModelIndex()+__OME__tmpLoop_7_i]=(OME_SCALAR)__OME__tempArray_1[__OME__tmpLoop_7_i];

}
OME_SCALAR __OME__tempArray_2[]={33.0,49.0,65.0,81.0,97.0,17.0,113.0,129.0,145.0,161.0,177.0,193.0,209.0,1.0,34.0,50.0,66.0,82.0,98.0,18.0,114.0,130.0,146.0,162.0,178.0,194.0,210.0,2.0,35.0,51.0,67.0,83.0,99.0,19.0,115.0,131.0,147.0,163.0,179.0,195.0,211.0,3.0,36.0,52.0,68.0,84.0,100.0,20.0,116.0,132.0,148.0,164.0,180.0,196.0,212.0,4.0,37.0,53.0,69.0,85.0,101.0,21.0,117.0,133.0,149.0,165.0,181.0,197.0,213.0,5.0,38.0,54.0,70.0,86.0,102.0,22.0,118.0,134.0,150.0,166.0,182.0,198.0,214.0,6.0,41.0,57.0,73.0,89.0,105.0,25.0,121.0,137.0,153.0,169.0,185.0,201.0,217.0,9.0,42.0,58.0,74.0,90.0,106.0,26.0,122.0,138.0,154.0,170.0,186.0,202.0,218.0,10.0,43.0,59.0,75.0,91.0,107.0,27.0,123.0,139.0,155.0,171.0,187.0,203.0,219.0,11.0,44.0,60.0,76.0,92.0,108.0,28.0,124.0,140.0,156.0,172.0,188.0,204.0,220.0,12.0,45.0,61.0,77.0,93.0,109.0,29.0,125.0,141.0,157.0,173.0,189.0,205.0,221.0,13.0,46.0,62.0,78.0,94.0,110.0,30.0,126.0,142.0,158.0,174.0,190.0,206.0,222.0,14.0};
size_t __OME__tempArray_2_dims[]={168};
for(size_t __OME__tmpLoop_8_i=0;__OME__tmpLoop_8_i<168;++__OME__tmpLoop_8_i)
{

//=-=-=-=-=-=-=-=-=-= node03598 (None_White)
(*root.GetValueArchive())[node03598.GetModelIndex()+__OME__tmpLoop_8_i]=(OME_SCALAR)__OME__tempArray_2[__OME__tmpLoop_8_i];

}
OME_SCALAR __OME__tempArray_3[]={145.0,161.0,177.0,193.0,209.0,146.0,162.0,178.0,194.0,210.0,147.0,163.0,179.0,195.0,211.0,148.0,164.0,180.0,196.0,212.0,149.0,165.0,181.0,197.0,213.0,150.0,166.0,182.0,198.0,214.0,151.0,167.0,183.0,199.0,215.0,152.0,168.0,184.0,200.0,216.0,153.0,169.0,185.0,201.0,217.0,154.0,170.0,186.0,202.0,218.0,155.0,171.0,187.0,203.0,219.0,156.0,172.0,188.0,204.0,220.0,157.0,173.0,189.0,205.0,221.0,158.0,174.0,190.0,206.0,222.0,159.0,175.0,191.0,207.0,223.0,160.0,176.0,192.0,208.0,224.0};
size_t __OME__tempArray_3_dims[]={80};
for(size_t __OME__tmpLoop_9_i=0;__OME__tmpLoop_9_i<80;++__OME__tmpLoop_9_i)
{

//=-=-=-=-=-=-=-=-=-= node03600 (Elderly)
(*root.GetValueArchive())[node03600.GetModelIndex()+__OME__tmpLoop_9_i]=(OME_SCALAR)__OME__tempArray_3[__OME__tmpLoop_9_i];

}
OME_SCALAR __OME__firsttrue_0;
OME_SCALAR __OME__tempArray_4[]={31.0,59.0,90.0,120.0,151.0,181.0,212.0,243.0,273.0,304.0,334.0,365.0};
size_t __OME__tempArray_4_dims[]={12};
__OME__firsttrue_0=0;
for(size_t __OME__tmpLoop_10_i=0;__OME__tmpLoop_10_i<12;++__OME__tmpLoop_10_i)
{
node03606.SetCurrentInstance(__OME__tmpLoop_10_i);


if(!__OME__firsttrue_0 && node03638<=__OME__tempArray_4[__OME__tmpLoop_10_i]) __OME__firsttrue_0=__OME__tmpLoop_10_i+1;
}
node03606.SetCurrentInstance(0);

OME_SCALAR __OME__firsttrue_1;
OME_SCALAR __OME__tempArray_5[]={8.0,21.0,35.0,49.0,63.0,77.0,91.0,105.0,119.0,133.0,147.0,161.0,175.0,189.0,203.0,217.0,231.0,245.0,259.0,273.0,287.0,301.0,315.0,329.0,343.0,357.0,372.0};
size_t __OME__tempArray_5_dims[]={27};
__OME__firsttrue_1=0;
for(size_t __OME__tmpLoop_11_i=0;__OME__tmpLoop_11_i<27;++__OME__tmpLoop_11_i)
{

if(!__OME__firsttrue_1 && node03638<=__OME__tempArray_5[__OME__tmpLoop_11_i]) __OME__firsttrue_1=__OME__tmpLoop_11_i+1;
}
OME_SCALAR __OME__tempArray_6[]={10000.0,15000.0,20000.0,25000.0,30000.0,35000.0,40000.0,45000.0,50000.0,60000.0,70000.0,100000.0,125000.0,150000.0,200000.0,20000000.0};
size_t __OME__tempArray_6_dims[]={16};
for(size_t __OME__tmpLoop_12_i=0;__OME__tmpLoop_12_i<16;++__OME__tmpLoop_12_i)
{

//=-=-=-=-=-=-=-=-=-= node03674 (Wealth_Upper_boundaries)
(*root.GetValueArchive())[node03674.GetModelIndex()+__OME__tmpLoop_12_i]=(OME_SCALAR)__OME__tempArray_6[__OME__tmpLoop_12_i];

}
OME_SCALAR __OME__firsttrue_2;
OME_SCALAR __OME__tempArray_7[]={99.5,97.5,90.0,75.0,50.0,25.0,10.0,2.5,0.5,0.1,0.0};
size_t __OME__tempArray_7_dims[]={11};
__OME__firsttrue_2=0;
for(size_t __OME__tmpLoop_13_i=0;__OME__tmpLoop_13_i<11;++__OME__tmpLoop_13_i)
{

if(!__OME__firsttrue_2 && node03702>=__OME__tempArray_7[__OME__tmpLoop_13_i]) __OME__firsttrue_2=__OME__tmpLoop_13_i+1;
}
//=-=-=-=-=-=-=-=-=-= node03614 (Biweely_period)
node03614=(OME_SCALAR)__OME__firsttrue_1;

//=-=-=-=-=-=-=-=-=-= node03712 (NDVI_data_or_predictions)
node03712=(OME_SCALAR)(node03622<32.0||node03622>=40.0||node03622==39.0&&node03614>19.0 ? (OME_SCALAR)1.0 : (OME_SCALAR)((node03605==0.0 ? (OME_SCALAR)0.0 : (OME_SCALAR)1.0)));

//=-=-=-=-=-=-=-=-=-= node03697 (NDVI_year)
node03697=(OME_SCALAR)node03622-31.0;

//=-=-=-=-=-=-=-=-=-= node03709 (Averaged_RA_Temp)
node03709=(OME_SCALAR)interpTable(node03709,TestModelBm,(node03638));

//=-=-=-=-=-=-=-=-=-= node03685 (RA_Rel_Hum)
node03685=(OME_SCALAR)node03663/(node03682+1.0);

//=-=-=-=-=-=-=-=-=-= node03683 (RA_Temp)
node03683=(OME_SCALAR)node03646/node03682;

for(size_t node03711_i=0;node03711_i<9;++node03711_i)
{
node03711.SetCurrentInstance(node03711_i);

//=-=-=-=-=-=-=-=-=-= node04119 (Temp_limit)
node04119=(OME_SCALAR)OMEMin((0.0),(node04123-node03683));

//=-=-=-=-=-=-=-=-=-= node04115 (NDVI_predicted)
node04115=(OME_SCALAR)node04118+node04122*node03709+node04117*node03683+node04121*node03685+node04124*node03672*node04119;

//=-=-=-=-=-=-=-=-=-= node03699 (NDVIs)
(*root.GetValueArchive())[node03699.GetModelIndex()+node03711_i]=(OME_SCALAR)(node03712==1.0 ? (OME_SCALAR)0.0 : (OME_SCALAR)element(element(node04091,node03697,3),node03614)[node03711_i]);

//=-=-=-=-=-=-=-=-=-= node04130 (NDVI)
node04130=(OME_SCALAR)(node03712==0.0 ? (OME_SCALAR)element(node03699,node03711_i+1,1) : (OME_SCALAR)node04115);

}
node03711.SetCurrentInstance(0);

for(size_t node03611_i=0;node03611_i<6;++node03611_i)
{
node03611.SetCurrentInstance(node03611_i);

OME_SCALAR __OME__tempArray_12[]={5.64,1276.4,8986.57,8986.57,2199.92,6000.11};
size_t __OME__tempArray_12_dims[]={6};
//=-=-=-=-=-=-=-=-=-= node03790 (pollutant_removal_unit_value)
node03790=(OME_SCALAR)element((OME_SCALAR*)__OME__tempArray_12,__OME__tempArray_12_dims,node03611_i+1,1);

OME_SCALAR __OME__tempArray_13[]={9812.19,2.47466,10.017,38.7003,5.47233,25.8955};
size_t __OME__tempArray_13_dims[]={6};
//=-=-=-=-=-=-=-=-=-= node03788 (__pollutant_removal_rate)
node03788=(OME_SCALAR)element((OME_SCALAR*)__OME__tempArray_13,__OME__tempArray_13_dims,node03611_i+1,1)/365.0;

}
node03611.SetCurrentInstance(0);


for(size_t node03595_i=0;node03595_i<10;++node03595_i)
{
node03595.SetCurrentInstance(node03595_i);




}
node03595.SetCurrentInstance(0);



//=-=-=-=-=-=-=-=-=-= node03612 (Choice_of_weather_data)
node03612=(OME_SCALAR)(node03622<27.0 ? (OME_SCALAR)0.0 : (OME_SCALAR)((node03622<41.0&&node03671==0.0 ? (OME_SCALAR)0.0 : (OME_SCALAR)1.0)));

//=-=-=-=-=-=-=-=-=-= node03604 (Air_T)
node03604=(OME_SCALAR)(currenttime(TestModelBm)==0.0 ? (OME_SCALAR)0.0 : (OME_SCALAR)((node03612==0.0 ? (OME_SCALAR)element(element(node03783,node03638,2),node03622) : (OME_SCALAR)node03653))+4.55);

//=-=-=-=-=-=-=-=-=-= node03632 (Bare_Soil_Temperature)
node03632=(OME_SCALAR)(node03604-3.1876)/0.6443;

for(size_t node03627_i=0;node03627_i<15;++node03627_i)
{
node03627.SetCurrentInstance(node03627_i);

__OME__sum_31[node03627_i]= 0.0;
__OME__sum_30[node03627_i]= 0.0;
__OME__sum_29[node03627_i]= 0.0;
//=-=-=-=-=-=-=-=-=-= node03804 (Temperature_buffer_capacity)
node03804=(OME_SCALAR)(node03627_i+1==1.0 ? (OME_SCALAR)node03607 : (OME_SCALAR)0.0);

OME_SCALAR __OME__tempArray_11[]={0.0,0.9,0.65,0.35,0.1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
size_t __OME__tempArray_11_dims[]={15};
//=-=-=-=-=-=-=-=-=-= node03812 (Pervious)
node03812=(OME_SCALAR)element((OME_SCALAR*)__OME__tempArray_11,__OME__tempArray_11_dims,node03627_i+1,1);

OME_SCALAR __OME__tempArray_14[]={0.0,node03814*elementRaw(node04130,2.0,1)+(node03812-node03814)*elementRaw(node04130,5.0,1),node03814*elementRaw(node04130,2.0,1)+(node03812-node03814)*elementRaw(node04130,5.0,1),node03814*elementRaw(node04130,2.0,1)+(node03812-node03814)*elementRaw(node04130,5.0,1),node03814*elementRaw(node04130,2.0,1)+(node03812-node03814)*elementRaw(node04130,5.0,1),0.06,elementRaw(node04130,1.0,1),elementRaw(node04130,2.0,1),elementRaw(node04130,3.0,1),elementRaw(node04130,4.0,1),elementRaw(node04130,5.0,1),elementRaw(node04130,6.0,1),elementRaw(node04130,7.0,1),elementRaw(node04130,8.0,1),elementRaw(node04130,9.0,1)};
size_t __OME__tempArray_14_dims[]={15};
//=-=-=-=-=-=-=-=-=-= node03820 (NDVI)
node03820=(OME_SCALAR)element((OME_SCALAR*)__OME__tempArray_14,__OME__tempArray_14_dims,node03627_i+1,1);

//=-=-=-=-=-=-=-=-=-= node03973 (Net_radiant_Energy)
node03973=(OME_SCALAR)node03821*29.17*(node03632-node03604);

}
node03627.SetCurrentInstance(0);

//=-=-=-=-=-=-=-=-=-= node00228 (Total_area)
node00228=(OME_SCALAR)__OME__sum_2;

//=-=-=-=-=-=-=-=-=-= node03602 (total_population)
node03602=(OME_SCALAR)__OME__sum_3;

//=-=-=-=-=-=-=-=-=-= node03637 (Choose_LandCover)
node03637.Evaluate(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03620 (R_Humidity)
node03620=(OME_SCALAR)(currenttime(TestModelBm)==0.0 ? (OME_SCALAR)0.0 : (OME_SCALAR)((node03612==0.0 ? (OME_SCALAR)element(element(node03784,node03638,2),node03622) : (OME_SCALAR)node03651)));

//=-=-=-=-=-=-=-=-=-= node03634 (Saturated_pressure)
node03634=(OME_SCALAR)0.61078*OMECFuncs::exp(17.269*node03632/(237.3+node03632));

//=-=-=-=-=-=-=-=-=-= node03628 (VPD)
node03628=(OME_SCALAR)node03634*(1.0-node03620/100.0);

//=-=-=-=-=-=-=-=-=-= node03630 (temp_reduction_per_unit_of_NDVI)
node03630=(OME_SCALAR)39.69*node03628/(12.21+node03628);

for(size_t node03644_i=0;node03644_i<696;++node03644_i)
{
node03644.SetCurrentInstance(node03644_i);
node03642.SetCurrentInstance(node03644_i);

OME_SCALAR __OME__sum_7[224];
size_t __OME__sum_7_dims[]={224};
for(size_t __OME__iniLoop_5_i=0;__OME__iniLoop_5_i<224;++__OME__iniLoop_5_i)
{

__OME__sum_7[__OME__iniLoop_5_i]= 0.0;
}
OME_SCALAR __OME__sum_4;
OME_SCALAR __OME__tmp_listGen_0[80]={};
__OME__tmp_listGen_0[0]=elementRaw(node04019,node03644_i+1,2)[node03600[0]-1];
__OME__tmp_listGen_0[1]=elementRaw(node04019,node03644_i+1,2)[node03600[1]-1];
__OME__tmp_listGen_0[2]=elementRaw(node04019,node03644_i+1,2)[node03600[2]-1];
__OME__tmp_listGen_0[3]=elementRaw(node04019,node03644_i+1,2)[node03600[3]-1];
__OME__tmp_listGen_0[4]=elementRaw(node04019,node03644_i+1,2)[node03600[4]-1];
__OME__tmp_listGen_0[5]=elementRaw(node04019,node03644_i+1,2)[node03600[5]-1];
__OME__tmp_listGen_0[6]=elementRaw(node04019,node03644_i+1,2)[node03600[6]-1];
__OME__tmp_listGen_0[7]=elementRaw(node04019,node03644_i+1,2)[node03600[7]-1];
__OME__tmp_listGen_0[8]=elementRaw(node04019,node03644_i+1,2)[node03600[8]-1];
__OME__tmp_listGen_0[9]=elementRaw(node04019,node03644_i+1,2)[node03600[9]-1];
__OME__tmp_listGen_0[10]=elementRaw(node04019,node03644_i+1,2)[node03600[10]-1];
__OME__tmp_listGen_0[11]=elementRaw(node04019,node03644_i+1,2)[node03600[11]-1];
__OME__tmp_listGen_0[12]=elementRaw(node04019,node03644_i+1,2)[node03600[12]-1];
__OME__tmp_listGen_0[13]=elementRaw(node04019,node03644_i+1,2)[node03600[13]-1];
__OME__tmp_listGen_0[14]=elementRaw(node04019,node03644_i+1,2)[node03600[14]-1];
__OME__tmp_listGen_0[15]=elementRaw(node04019,node03644_i+1,2)[node03600[15]-1];
__OME__tmp_listGen_0[16]=elementRaw(node04019,node03644_i+1,2)[node03600[16]-1];
__OME__tmp_listGen_0[17]=elementRaw(node04019,node03644_i+1,2)[node03600[17]-1];
__OME__tmp_listGen_0[18]=elementRaw(node04019,node03644_i+1,2)[node03600[18]-1];
__OME__tmp_listGen_0[19]=elementRaw(node04019,node03644_i+1,2)[node03600[19]-1];
__OME__tmp_listGen_0[20]=elementRaw(node04019,node03644_i+1,2)[node03600[20]-1];
__OME__tmp_listGen_0[21]=elementRaw(node04019,node03644_i+1,2)[node03600[21]-1];
__OME__tmp_listGen_0[22]=elementRaw(node04019,node03644_i+1,2)[node03600[22]-1];
__OME__tmp_listGen_0[23]=elementRaw(node04019,node03644_i+1,2)[node03600[23]-1];
__OME__tmp_listGen_0[24]=elementRaw(node04019,node03644_i+1,2)[node03600[24]-1];
__OME__tmp_listGen_0[25]=elementRaw(node04019,node03644_i+1,2)[node03600[25]-1];
__OME__tmp_listGen_0[26]=elementRaw(node04019,node03644_i+1,2)[node03600[26]-1];
__OME__tmp_listGen_0[27]=elementRaw(node04019,node03644_i+1,2)[node03600[27]-1];
__OME__tmp_listGen_0[28]=elementRaw(node04019,node03644_i+1,2)[node03600[28]-1];
__OME__tmp_listGen_0[29]=elementRaw(node04019,node03644_i+1,2)[node03600[29]-1];
__OME__tmp_listGen_0[30]=elementRaw(node04019,node03644_i+1,2)[node03600[30]-1];
__OME__tmp_listGen_0[31]=elementRaw(node04019,node03644_i+1,2)[node03600[31]-1];
__OME__tmp_listGen_0[32]=elementRaw(node04019,node03644_i+1,2)[node03600[32]-1];
__OME__tmp_listGen_0[33]=elementRaw(node04019,node03644_i+1,2)[node03600[33]-1];
__OME__tmp_listGen_0[34]=elementRaw(node04019,node03644_i+1,2)[node03600[34]-1];
__OME__tmp_listGen_0[35]=elementRaw(node04019,node03644_i+1,2)[node03600[35]-1];
__OME__tmp_listGen_0[36]=elementRaw(node04019,node03644_i+1,2)[node03600[36]-1];
__OME__tmp_listGen_0[37]=elementRaw(node04019,node03644_i+1,2)[node03600[37]-1];
__OME__tmp_listGen_0[38]=elementRaw(node04019,node03644_i+1,2)[node03600[38]-1];
__OME__tmp_listGen_0[39]=elementRaw(node04019,node03644_i+1,2)[node03600[39]-1];
__OME__tmp_listGen_0[40]=elementRaw(node04019,node03644_i+1,2)[node03600[40]-1];
__OME__tmp_listGen_0[41]=elementRaw(node04019,node03644_i+1,2)[node03600[41]-1];
__OME__tmp_listGen_0[42]=elementRaw(node04019,node03644_i+1,2)[node03600[42]-1];
__OME__tmp_listGen_0[43]=elementRaw(node04019,node03644_i+1,2)[node03600[43]-1];
__OME__tmp_listGen_0[44]=elementRaw(node04019,node03644_i+1,2)[node03600[44]-1];
__OME__tmp_listGen_0[45]=elementRaw(node04019,node03644_i+1,2)[node03600[45]-1];
__OME__tmp_listGen_0[46]=elementRaw(node04019,node03644_i+1,2)[node03600[46]-1];
__OME__tmp_listGen_0[47]=elementRaw(node04019,node03644_i+1,2)[node03600[47]-1];
__OME__tmp_listGen_0[48]=elementRaw(node04019,node03644_i+1,2)[node03600[48]-1];
__OME__tmp_listGen_0[49]=elementRaw(node04019,node03644_i+1,2)[node03600[49]-1];
__OME__tmp_listGen_0[50]=elementRaw(node04019,node03644_i+1,2)[node03600[50]-1];
__OME__tmp_listGen_0[51]=elementRaw(node04019,node03644_i+1,2)[node03600[51]-1];
__OME__tmp_listGen_0[52]=elementRaw(node04019,node03644_i+1,2)[node03600[52]-1];
__OME__tmp_listGen_0[53]=elementRaw(node04019,node03644_i+1,2)[node03600[53]-1];
__OME__tmp_listGen_0[54]=elementRaw(node04019,node03644_i+1,2)[node03600[54]-1];
__OME__tmp_listGen_0[55]=elementRaw(node04019,node03644_i+1,2)[node03600[55]-1];
__OME__tmp_listGen_0[56]=elementRaw(node04019,node03644_i+1,2)[node03600[56]-1];
__OME__tmp_listGen_0[57]=elementRaw(node04019,node03644_i+1,2)[node03600[57]-1];
__OME__tmp_listGen_0[58]=elementRaw(node04019,node03644_i+1,2)[node03600[58]-1];
__OME__tmp_listGen_0[59]=elementRaw(node04019,node03644_i+1,2)[node03600[59]-1];
__OME__tmp_listGen_0[60]=elementRaw(node04019,node03644_i+1,2)[node03600[60]-1];
__OME__tmp_listGen_0[61]=elementRaw(node04019,node03644_i+1,2)[node03600[61]-1];
__OME__tmp_listGen_0[62]=elementRaw(node04019,node03644_i+1,2)[node03600[62]-1];
__OME__tmp_listGen_0[63]=elementRaw(node04019,node03644_i+1,2)[node03600[63]-1];
__OME__tmp_listGen_0[64]=elementRaw(node04019,node03644_i+1,2)[node03600[64]-1];
__OME__tmp_listGen_0[65]=elementRaw(node04019,node03644_i+1,2)[node03600[65]-1];
__OME__tmp_listGen_0[66]=elementRaw(node04019,node03644_i+1,2)[node03600[66]-1];
__OME__tmp_listGen_0[67]=elementRaw(node04019,node03644_i+1,2)[node03600[67]-1];
__OME__tmp_listGen_0[68]=elementRaw(node04019,node03644_i+1,2)[node03600[68]-1];
__OME__tmp_listGen_0[69]=elementRaw(node04019,node03644_i+1,2)[node03600[69]-1];
__OME__tmp_listGen_0[70]=elementRaw(node04019,node03644_i+1,2)[node03600[70]-1];
__OME__tmp_listGen_0[71]=elementRaw(node04019,node03644_i+1,2)[node03600[71]-1];
__OME__tmp_listGen_0[72]=elementRaw(node04019,node03644_i+1,2)[node03600[72]-1];
__OME__tmp_listGen_0[73]=elementRaw(node04019,node03644_i+1,2)[node03600[73]-1];
__OME__tmp_listGen_0[74]=elementRaw(node04019,node03644_i+1,2)[node03600[74]-1];
__OME__tmp_listGen_0[75]=elementRaw(node04019,node03644_i+1,2)[node03600[75]-1];
__OME__tmp_listGen_0[76]=elementRaw(node04019,node03644_i+1,2)[node03600[76]-1];
__OME__tmp_listGen_0[77]=elementRaw(node04019,node03644_i+1,2)[node03600[77]-1];
__OME__tmp_listGen_0[78]=elementRaw(node04019,node03644_i+1,2)[node03600[78]-1];
__OME__tmp_listGen_0[79]=elementRaw(node04019,node03644_i+1,2)[node03600[79]-1];

__OME__sum_4= 0.0;
for(size_t __OME__tmpLoop_16_i=0;__OME__tmpLoop_16_i<80;++__OME__tmpLoop_16_i)
{

__OME__sum_4+=__OME__tmp_listGen_0[__OME__tmpLoop_16_i];
}
OME_SCALAR __OME__sum_5;
OME_SCALAR __OME__tmp_listGen_1[48]={};
__OME__tmp_listGen_1[0]=elementRaw(node04019,node03644_i+1,2)[node03596[0]-1];
__OME__tmp_listGen_1[1]=elementRaw(node04019,node03644_i+1,2)[node03596[1]-1];
__OME__tmp_listGen_1[2]=elementRaw(node04019,node03644_i+1,2)[node03596[2]-1];
__OME__tmp_listGen_1[3]=elementRaw(node04019,node03644_i+1,2)[node03596[3]-1];
__OME__tmp_listGen_1[4]=elementRaw(node04019,node03644_i+1,2)[node03596[4]-1];
__OME__tmp_listGen_1[5]=elementRaw(node04019,node03644_i+1,2)[node03596[5]-1];
__OME__tmp_listGen_1[6]=elementRaw(node04019,node03644_i+1,2)[node03596[6]-1];
__OME__tmp_listGen_1[7]=elementRaw(node04019,node03644_i+1,2)[node03596[7]-1];
__OME__tmp_listGen_1[8]=elementRaw(node04019,node03644_i+1,2)[node03596[8]-1];
__OME__tmp_listGen_1[9]=elementRaw(node04019,node03644_i+1,2)[node03596[9]-1];
__OME__tmp_listGen_1[10]=elementRaw(node04019,node03644_i+1,2)[node03596[10]-1];
__OME__tmp_listGen_1[11]=elementRaw(node04019,node03644_i+1,2)[node03596[11]-1];
__OME__tmp_listGen_1[12]=elementRaw(node04019,node03644_i+1,2)[node03596[12]-1];
__OME__tmp_listGen_1[13]=elementRaw(node04019,node03644_i+1,2)[node03596[13]-1];
__OME__tmp_listGen_1[14]=elementRaw(node04019,node03644_i+1,2)[node03596[14]-1];
__OME__tmp_listGen_1[15]=elementRaw(node04019,node03644_i+1,2)[node03596[15]-1];
__OME__tmp_listGen_1[16]=elementRaw(node04019,node03644_i+1,2)[node03596[16]-1];
__OME__tmp_listGen_1[17]=elementRaw(node04019,node03644_i+1,2)[node03596[17]-1];
__OME__tmp_listGen_1[18]=elementRaw(node04019,node03644_i+1,2)[node03596[18]-1];
__OME__tmp_listGen_1[19]=elementRaw(node04019,node03644_i+1,2)[node03596[19]-1];
__OME__tmp_listGen_1[20]=elementRaw(node04019,node03644_i+1,2)[node03596[20]-1];
__OME__tmp_listGen_1[21]=elementRaw(node04019,node03644_i+1,2)[node03596[21]-1];
__OME__tmp_listGen_1[22]=elementRaw(node04019,node03644_i+1,2)[node03596[22]-1];
__OME__tmp_listGen_1[23]=elementRaw(node04019,node03644_i+1,2)[node03596[23]-1];
__OME__tmp_listGen_1[24]=elementRaw(node04019,node03644_i+1,2)[node03596[24]-1];
__OME__tmp_listGen_1[25]=elementRaw(node04019,node03644_i+1,2)[node03596[25]-1];
__OME__tmp_listGen_1[26]=elementRaw(node04019,node03644_i+1,2)[node03596[26]-1];
__OME__tmp_listGen_1[27]=elementRaw(node04019,node03644_i+1,2)[node03596[27]-1];
__OME__tmp_listGen_1[28]=elementRaw(node04019,node03644_i+1,2)[node03596[28]-1];
__OME__tmp_listGen_1[29]=elementRaw(node04019,node03644_i+1,2)[node03596[29]-1];
__OME__tmp_listGen_1[30]=elementRaw(node04019,node03644_i+1,2)[node03596[30]-1];
__OME__tmp_listGen_1[31]=elementRaw(node04019,node03644_i+1,2)[node03596[31]-1];
__OME__tmp_listGen_1[32]=elementRaw(node04019,node03644_i+1,2)[node03596[32]-1];
__OME__tmp_listGen_1[33]=elementRaw(node04019,node03644_i+1,2)[node03596[33]-1];
__OME__tmp_listGen_1[34]=elementRaw(node04019,node03644_i+1,2)[node03596[34]-1];
__OME__tmp_listGen_1[35]=elementRaw(node04019,node03644_i+1,2)[node03596[35]-1];
__OME__tmp_listGen_1[36]=elementRaw(node04019,node03644_i+1,2)[node03596[36]-1];
__OME__tmp_listGen_1[37]=elementRaw(node04019,node03644_i+1,2)[node03596[37]-1];
__OME__tmp_listGen_1[38]=elementRaw(node04019,node03644_i+1,2)[node03596[38]-1];
__OME__tmp_listGen_1[39]=elementRaw(node04019,node03644_i+1,2)[node03596[39]-1];
__OME__tmp_listGen_1[40]=elementRaw(node04019,node03644_i+1,2)[node03596[40]-1];
__OME__tmp_listGen_1[41]=elementRaw(node04019,node03644_i+1,2)[node03596[41]-1];
__OME__tmp_listGen_1[42]=elementRaw(node04019,node03644_i+1,2)[node03596[42]-1];
__OME__tmp_listGen_1[43]=elementRaw(node04019,node03644_i+1,2)[node03596[43]-1];
__OME__tmp_listGen_1[44]=elementRaw(node04019,node03644_i+1,2)[node03596[44]-1];
__OME__tmp_listGen_1[45]=elementRaw(node04019,node03644_i+1,2)[node03596[45]-1];
__OME__tmp_listGen_1[46]=elementRaw(node04019,node03644_i+1,2)[node03596[46]-1];
__OME__tmp_listGen_1[47]=elementRaw(node04019,node03644_i+1,2)[node03596[47]-1];

__OME__sum_5= 0.0;
for(size_t __OME__tmpLoop_17_i=0;__OME__tmpLoop_17_i<48;++__OME__tmpLoop_17_i)
{

__OME__sum_5+=__OME__tmp_listGen_1[__OME__tmpLoop_17_i];
}
for(size_t __OME__tmpLoop_18_i=0;__OME__tmpLoop_18_i<696;++__OME__tmpLoop_18_i)
{

for(size_t __OME__tmpLoop_19_i=0;__OME__tmpLoop_19_i<224;++__OME__tmpLoop_19_i)
{

__OME__sum_7[__OME__tmpLoop_19_i]+=(*root.GetValueArchive())[node04019.GetModelIndex()+__OME__tmpLoop_19_i+224*(__OME__tmpLoop_18_i)];
}
}
OME_SCALAR __OME__sum_6;
OME_SCALAR __OME__tmp_listGen_2[168]={};
__OME__tmp_listGen_2[0]=elementRaw(node04019,node03644_i+1,2)[node03598[0]-1];
__OME__tmp_listGen_2[1]=elementRaw(node04019,node03644_i+1,2)[node03598[1]-1];
__OME__tmp_listGen_2[2]=elementRaw(node04019,node03644_i+1,2)[node03598[2]-1];
__OME__tmp_listGen_2[3]=elementRaw(node04019,node03644_i+1,2)[node03598[3]-1];
__OME__tmp_listGen_2[4]=elementRaw(node04019,node03644_i+1,2)[node03598[4]-1];
__OME__tmp_listGen_2[5]=elementRaw(node04019,node03644_i+1,2)[node03598[5]-1];
__OME__tmp_listGen_2[6]=elementRaw(node04019,node03644_i+1,2)[node03598[6]-1];
__OME__tmp_listGen_2[7]=elementRaw(node04019,node03644_i+1,2)[node03598[7]-1];
__OME__tmp_listGen_2[8]=elementRaw(node04019,node03644_i+1,2)[node03598[8]-1];
__OME__tmp_listGen_2[9]=elementRaw(node04019,node03644_i+1,2)[node03598[9]-1];
__OME__tmp_listGen_2[10]=elementRaw(node04019,node03644_i+1,2)[node03598[10]-1];
__OME__tmp_listGen_2[11]=elementRaw(node04019,node03644_i+1,2)[node03598[11]-1];
__OME__tmp_listGen_2[12]=elementRaw(node04019,node03644_i+1,2)[node03598[12]-1];
__OME__tmp_listGen_2[13]=elementRaw(node04019,node03644_i+1,2)[node03598[13]-1];
__OME__tmp_listGen_2[14]=elementRaw(node04019,node03644_i+1,2)[node03598[14]-1];
__OME__tmp_listGen_2[15]=elementRaw(node04019,node03644_i+1,2)[node03598[15]-1];
__OME__tmp_listGen_2[16]=elementRaw(node04019,node03644_i+1,2)[node03598[16]-1];
__OME__tmp_listGen_2[17]=elementRaw(node04019,node03644_i+1,2)[node03598[17]-1];
__OME__tmp_listGen_2[18]=elementRaw(node04019,node03644_i+1,2)[node03598[18]-1];
__OME__tmp_listGen_2[19]=elementRaw(node04019,node03644_i+1,2)[node03598[19]-1];
__OME__tmp_listGen_2[20]=elementRaw(node04019,node03644_i+1,2)[node03598[20]-1];
__OME__tmp_listGen_2[21]=elementRaw(node04019,node03644_i+1,2)[node03598[21]-1];
__OME__tmp_listGen_2[22]=elementRaw(node04019,node03644_i+1,2)[node03598[22]-1];
__OME__tmp_listGen_2[23]=elementRaw(node04019,node03644_i+1,2)[node03598[23]-1];
__OME__tmp_listGen_2[24]=elementRaw(node04019,node03644_i+1,2)[node03598[24]-1];
__OME__tmp_listGen_2[25]=elementRaw(node04019,node03644_i+1,2)[node03598[25]-1];
__OME__tmp_listGen_2[26]=elementRaw(node04019,node03644_i+1,2)[node03598[26]-1];
__OME__tmp_listGen_2[27]=elementRaw(node04019,node03644_i+1,2)[node03598[27]-1];
__OME__tmp_listGen_2[28]=elementRaw(node04019,node03644_i+1,2)[node03598[28]-1];
__OME__tmp_listGen_2[29]=elementRaw(node04019,node03644_i+1,2)[node03598[29]-1];
__OME__tmp_listGen_2[30]=elementRaw(node04019,node03644_i+1,2)[node03598[30]-1];
__OME__tmp_listGen_2[31]=elementRaw(node04019,node03644_i+1,2)[node03598[31]-1];
__OME__tmp_listGen_2[32]=elementRaw(node04019,node03644_i+1,2)[node03598[32]-1];
__OME__tmp_listGen_2[33]=elementRaw(node04019,node03644_i+1,2)[node03598[33]-1];
__OME__tmp_listGen_2[34]=elementRaw(node04019,node03644_i+1,2)[node03598[34]-1];
__OME__tmp_listGen_2[35]=elementRaw(node04019,node03644_i+1,2)[node03598[35]-1];
__OME__tmp_listGen_2[36]=elementRaw(node04019,node03644_i+1,2)[node03598[36]-1];
__OME__tmp_listGen_2[37]=elementRaw(node04019,node03644_i+1,2)[node03598[37]-1];
__OME__tmp_listGen_2[38]=elementRaw(node04019,node03644_i+1,2)[node03598[38]-1];
__OME__tmp_listGen_2[39]=elementRaw(node04019,node03644_i+1,2)[node03598[39]-1];
__OME__tmp_listGen_2[40]=elementRaw(node04019,node03644_i+1,2)[node03598[40]-1];
__OME__tmp_listGen_2[41]=elementRaw(node04019,node03644_i+1,2)[node03598[41]-1];
__OME__tmp_listGen_2[42]=elementRaw(node04019,node03644_i+1,2)[node03598[42]-1];
__OME__tmp_listGen_2[43]=elementRaw(node04019,node03644_i+1,2)[node03598[43]-1];
__OME__tmp_listGen_2[44]=elementRaw(node04019,node03644_i+1,2)[node03598[44]-1];
__OME__tmp_listGen_2[45]=elementRaw(node04019,node03644_i+1,2)[node03598[45]-1];
__OME__tmp_listGen_2[46]=elementRaw(node04019,node03644_i+1,2)[node03598[46]-1];
__OME__tmp_listGen_2[47]=elementRaw(node04019,node03644_i+1,2)[node03598[47]-1];
__OME__tmp_listGen_2[48]=elementRaw(node04019,node03644_i+1,2)[node03598[48]-1];
__OME__tmp_listGen_2[49]=elementRaw(node04019,node03644_i+1,2)[node03598[49]-1];
__OME__tmp_listGen_2[50]=elementRaw(node04019,node03644_i+1,2)[node03598[50]-1];
__OME__tmp_listGen_2[51]=elementRaw(node04019,node03644_i+1,2)[node03598[51]-1];
__OME__tmp_listGen_2[52]=elementRaw(node04019,node03644_i+1,2)[node03598[52]-1];
__OME__tmp_listGen_2[53]=elementRaw(node04019,node03644_i+1,2)[node03598[53]-1];
__OME__tmp_listGen_2[54]=elementRaw(node04019,node03644_i+1,2)[node03598[54]-1];
__OME__tmp_listGen_2[55]=elementRaw(node04019,node03644_i+1,2)[node03598[55]-1];
__OME__tmp_listGen_2[56]=elementRaw(node04019,node03644_i+1,2)[node03598[56]-1];
__OME__tmp_listGen_2[57]=elementRaw(node04019,node03644_i+1,2)[node03598[57]-1];
__OME__tmp_listGen_2[58]=elementRaw(node04019,node03644_i+1,2)[node03598[58]-1];
__OME__tmp_listGen_2[59]=elementRaw(node04019,node03644_i+1,2)[node03598[59]-1];
__OME__tmp_listGen_2[60]=elementRaw(node04019,node03644_i+1,2)[node03598[60]-1];
__OME__tmp_listGen_2[61]=elementRaw(node04019,node03644_i+1,2)[node03598[61]-1];
__OME__tmp_listGen_2[62]=elementRaw(node04019,node03644_i+1,2)[node03598[62]-1];
__OME__tmp_listGen_2[63]=elementRaw(node04019,node03644_i+1,2)[node03598[63]-1];
__OME__tmp_listGen_2[64]=elementRaw(node04019,node03644_i+1,2)[node03598[64]-1];
__OME__tmp_listGen_2[65]=elementRaw(node04019,node03644_i+1,2)[node03598[65]-1];
__OME__tmp_listGen_2[66]=elementRaw(node04019,node03644_i+1,2)[node03598[66]-1];
__OME__tmp_listGen_2[67]=elementRaw(node04019,node03644_i+1,2)[node03598[67]-1];
__OME__tmp_listGen_2[68]=elementRaw(node04019,node03644_i+1,2)[node03598[68]-1];
__OME__tmp_listGen_2[69]=elementRaw(node04019,node03644_i+1,2)[node03598[69]-1];
__OME__tmp_listGen_2[70]=elementRaw(node04019,node03644_i+1,2)[node03598[70]-1];
__OME__tmp_listGen_2[71]=elementRaw(node04019,node03644_i+1,2)[node03598[71]-1];
__OME__tmp_listGen_2[72]=elementRaw(node04019,node03644_i+1,2)[node03598[72]-1];
__OME__tmp_listGen_2[73]=elementRaw(node04019,node03644_i+1,2)[node03598[73]-1];
__OME__tmp_listGen_2[74]=elementRaw(node04019,node03644_i+1,2)[node03598[74]-1];
__OME__tmp_listGen_2[75]=elementRaw(node04019,node03644_i+1,2)[node03598[75]-1];
__OME__tmp_listGen_2[76]=elementRaw(node04019,node03644_i+1,2)[node03598[76]-1];
__OME__tmp_listGen_2[77]=elementRaw(node04019,node03644_i+1,2)[node03598[77]-1];
__OME__tmp_listGen_2[78]=elementRaw(node04019,node03644_i+1,2)[node03598[78]-1];
__OME__tmp_listGen_2[79]=elementRaw(node04019,node03644_i+1,2)[node03598[79]-1];
__OME__tmp_listGen_2[80]=elementRaw(node04019,node03644_i+1,2)[node03598[80]-1];
__OME__tmp_listGen_2[81]=elementRaw(node04019,node03644_i+1,2)[node03598[81]-1];
__OME__tmp_listGen_2[82]=elementRaw(node04019,node03644_i+1,2)[node03598[82]-1];
__OME__tmp_listGen_2[83]=elementRaw(node04019,node03644_i+1,2)[node03598[83]-1];
__OME__tmp_listGen_2[84]=elementRaw(node04019,node03644_i+1,2)[node03598[84]-1];
__OME__tmp_listGen_2[85]=elementRaw(node04019,node03644_i+1,2)[node03598[85]-1];
__OME__tmp_listGen_2[86]=elementRaw(node04019,node03644_i+1,2)[node03598[86]-1];
__OME__tmp_listGen_2[87]=elementRaw(node04019,node03644_i+1,2)[node03598[87]-1];
__OME__tmp_listGen_2[88]=elementRaw(node04019,node03644_i+1,2)[node03598[88]-1];
__OME__tmp_listGen_2[89]=elementRaw(node04019,node03644_i+1,2)[node03598[89]-1];
__OME__tmp_listGen_2[90]=elementRaw(node04019,node03644_i+1,2)[node03598[90]-1];
__OME__tmp_listGen_2[91]=elementRaw(node04019,node03644_i+1,2)[node03598[91]-1];
__OME__tmp_listGen_2[92]=elementRaw(node04019,node03644_i+1,2)[node03598[92]-1];
__OME__tmp_listGen_2[93]=elementRaw(node04019,node03644_i+1,2)[node03598[93]-1];
__OME__tmp_listGen_2[94]=elementRaw(node04019,node03644_i+1,2)[node03598[94]-1];
__OME__tmp_listGen_2[95]=elementRaw(node04019,node03644_i+1,2)[node03598[95]-1];
__OME__tmp_listGen_2[96]=elementRaw(node04019,node03644_i+1,2)[node03598[96]-1];
__OME__tmp_listGen_2[97]=elementRaw(node04019,node03644_i+1,2)[node03598[97]-1];
__OME__tmp_listGen_2[98]=elementRaw(node04019,node03644_i+1,2)[node03598[98]-1];
__OME__tmp_listGen_2[99]=elementRaw(node04019,node03644_i+1,2)[node03598[99]-1];
__OME__tmp_listGen_2[100]=elementRaw(node04019,node03644_i+1,2)[node03598[100]-1];
__OME__tmp_listGen_2[101]=elementRaw(node04019,node03644_i+1,2)[node03598[101]-1];
__OME__tmp_listGen_2[102]=elementRaw(node04019,node03644_i+1,2)[node03598[102]-1];
__OME__tmp_listGen_2[103]=elementRaw(node04019,node03644_i+1,2)[node03598[103]-1];
__OME__tmp_listGen_2[104]=elementRaw(node04019,node03644_i+1,2)[node03598[104]-1];
__OME__tmp_listGen_2[105]=elementRaw(node04019,node03644_i+1,2)[node03598[105]-1];
__OME__tmp_listGen_2[106]=elementRaw(node04019,node03644_i+1,2)[node03598[106]-1];
__OME__tmp_listGen_2[107]=elementRaw(node04019,node03644_i+1,2)[node03598[107]-1];
__OME__tmp_listGen_2[108]=elementRaw(node04019,node03644_i+1,2)[node03598[108]-1];
__OME__tmp_listGen_2[109]=elementRaw(node04019,node03644_i+1,2)[node03598[109]-1];
__OME__tmp_listGen_2[110]=elementRaw(node04019,node03644_i+1,2)[node03598[110]-1];
__OME__tmp_listGen_2[111]=elementRaw(node04019,node03644_i+1,2)[node03598[111]-1];
__OME__tmp_listGen_2[112]=elementRaw(node04019,node03644_i+1,2)[node03598[112]-1];
__OME__tmp_listGen_2[113]=elementRaw(node04019,node03644_i+1,2)[node03598[113]-1];
__OME__tmp_listGen_2[114]=elementRaw(node04019,node03644_i+1,2)[node03598[114]-1];
__OME__tmp_listGen_2[115]=elementRaw(node04019,node03644_i+1,2)[node03598[115]-1];
__OME__tmp_listGen_2[116]=elementRaw(node04019,node03644_i+1,2)[node03598[116]-1];
__OME__tmp_listGen_2[117]=elementRaw(node04019,node03644_i+1,2)[node03598[117]-1];
__OME__tmp_listGen_2[118]=elementRaw(node04019,node03644_i+1,2)[node03598[118]-1];
__OME__tmp_listGen_2[119]=elementRaw(node04019,node03644_i+1,2)[node03598[119]-1];
__OME__tmp_listGen_2[120]=elementRaw(node04019,node03644_i+1,2)[node03598[120]-1];
__OME__tmp_listGen_2[121]=elementRaw(node04019,node03644_i+1,2)[node03598[121]-1];
__OME__tmp_listGen_2[122]=elementRaw(node04019,node03644_i+1,2)[node03598[122]-1];
__OME__tmp_listGen_2[123]=elementRaw(node04019,node03644_i+1,2)[node03598[123]-1];
__OME__tmp_listGen_2[124]=elementRaw(node04019,node03644_i+1,2)[node03598[124]-1];
__OME__tmp_listGen_2[125]=elementRaw(node04019,node03644_i+1,2)[node03598[125]-1];
__OME__tmp_listGen_2[126]=elementRaw(node04019,node03644_i+1,2)[node03598[126]-1];
__OME__tmp_listGen_2[127]=elementRaw(node04019,node03644_i+1,2)[node03598[127]-1];
__OME__tmp_listGen_2[128]=elementRaw(node04019,node03644_i+1,2)[node03598[128]-1];
__OME__tmp_listGen_2[129]=elementRaw(node04019,node03644_i+1,2)[node03598[129]-1];
__OME__tmp_listGen_2[130]=elementRaw(node04019,node03644_i+1,2)[node03598[130]-1];
__OME__tmp_listGen_2[131]=elementRaw(node04019,node03644_i+1,2)[node03598[131]-1];
__OME__tmp_listGen_2[132]=elementRaw(node04019,node03644_i+1,2)[node03598[132]-1];
__OME__tmp_listGen_2[133]=elementRaw(node04019,node03644_i+1,2)[node03598[133]-1];
__OME__tmp_listGen_2[134]=elementRaw(node04019,node03644_i+1,2)[node03598[134]-1];
__OME__tmp_listGen_2[135]=elementRaw(node04019,node03644_i+1,2)[node03598[135]-1];
__OME__tmp_listGen_2[136]=elementRaw(node04019,node03644_i+1,2)[node03598[136]-1];
__OME__tmp_listGen_2[137]=elementRaw(node04019,node03644_i+1,2)[node03598[137]-1];
__OME__tmp_listGen_2[138]=elementRaw(node04019,node03644_i+1,2)[node03598[138]-1];
__OME__tmp_listGen_2[139]=elementRaw(node04019,node03644_i+1,2)[node03598[139]-1];
__OME__tmp_listGen_2[140]=elementRaw(node04019,node03644_i+1,2)[node03598[140]-1];
__OME__tmp_listGen_2[141]=elementRaw(node04019,node03644_i+1,2)[node03598[141]-1];
__OME__tmp_listGen_2[142]=elementRaw(node04019,node03644_i+1,2)[node03598[142]-1];
__OME__tmp_listGen_2[143]=elementRaw(node04019,node03644_i+1,2)[node03598[143]-1];
__OME__tmp_listGen_2[144]=elementRaw(node04019,node03644_i+1,2)[node03598[144]-1];
__OME__tmp_listGen_2[145]=elementRaw(node04019,node03644_i+1,2)[node03598[145]-1];
__OME__tmp_listGen_2[146]=elementRaw(node04019,node03644_i+1,2)[node03598[146]-1];
__OME__tmp_listGen_2[147]=elementRaw(node04019,node03644_i+1,2)[node03598[147]-1];
__OME__tmp_listGen_2[148]=elementRaw(node04019,node03644_i+1,2)[node03598[148]-1];
__OME__tmp_listGen_2[149]=elementRaw(node04019,node03644_i+1,2)[node03598[149]-1];
__OME__tmp_listGen_2[150]=elementRaw(node04019,node03644_i+1,2)[node03598[150]-1];
__OME__tmp_listGen_2[151]=elementRaw(node04019,node03644_i+1,2)[node03598[151]-1];
__OME__tmp_listGen_2[152]=elementRaw(node04019,node03644_i+1,2)[node03598[152]-1];
__OME__tmp_listGen_2[153]=elementRaw(node04019,node03644_i+1,2)[node03598[153]-1];
__OME__tmp_listGen_2[154]=elementRaw(node04019,node03644_i+1,2)[node03598[154]-1];
__OME__tmp_listGen_2[155]=elementRaw(node04019,node03644_i+1,2)[node03598[155]-1];
__OME__tmp_listGen_2[156]=elementRaw(node04019,node03644_i+1,2)[node03598[156]-1];
__OME__tmp_listGen_2[157]=elementRaw(node04019,node03644_i+1,2)[node03598[157]-1];
__OME__tmp_listGen_2[158]=elementRaw(node04019,node03644_i+1,2)[node03598[158]-1];
__OME__tmp_listGen_2[159]=elementRaw(node04019,node03644_i+1,2)[node03598[159]-1];
__OME__tmp_listGen_2[160]=elementRaw(node04019,node03644_i+1,2)[node03598[160]-1];
__OME__tmp_listGen_2[161]=elementRaw(node04019,node03644_i+1,2)[node03598[161]-1];
__OME__tmp_listGen_2[162]=elementRaw(node04019,node03644_i+1,2)[node03598[162]-1];
__OME__tmp_listGen_2[163]=elementRaw(node04019,node03644_i+1,2)[node03598[163]-1];
__OME__tmp_listGen_2[164]=elementRaw(node04019,node03644_i+1,2)[node03598[164]-1];
__OME__tmp_listGen_2[165]=elementRaw(node04019,node03644_i+1,2)[node03598[165]-1];
__OME__tmp_listGen_2[166]=elementRaw(node04019,node03644_i+1,2)[node03598[166]-1];
__OME__tmp_listGen_2[167]=elementRaw(node04019,node03644_i+1,2)[node03598[167]-1];

__OME__sum_6= 0.0;
for(size_t __OME__tmpLoop_20_i=0;__OME__tmpLoop_20_i<168;++__OME__tmpLoop_20_i)
{

__OME__sum_6+=__OME__tmp_listGen_2[__OME__tmpLoop_20_i];
}
OME_SCALAR __OME__sum_8;
__OME__sum_8= 0.0;
OME_SCALAR __OME__posgreatest_0;
__OME__posgreatest_0=0;
OME_SCALAR __OME__posgreatest_0_second=OME_SCALAR_MIN;
for(size_t node03988_i=0;node03988_i<15;++node03988_i)
{
node03988.SetCurrentInstance(node03988_i);

//=-=-=-=-=-=-=-=-=-= node04034 (Effect_on_wind)
node04034=(OME_SCALAR)node04060*elementRaw(node03809,node03988_i+1,1);

//=-=-=-=-=-=-=-=-=-= node04039 (NDVI)
node04039=(OME_SCALAR)elementRaw(node03820,node03988_i+1,1);

//=-=-=-=-=-=-=-=-=-= node04041 (weighted_NDVI)
node04041=(OME_SCALAR)node04039*node04060;

//=-=-=-=-=-=-=-=-=-= node04030 (Tree_cover_area)
node04030=(OME_SCALAR)node04060*node03970*elementRaw(node03814,node03988_i+1,1);

//=-=-=-=-=-=-=-=-=-= node04052 (temperature_reduction)
node04052=(OME_SCALAR)node03630*elementRaw(node03820,node03988_i+1,1)+elementRaw(node03804,node03988_i+1,1);

//=-=-=-=-=-=-=-=-=-= node04054 (Canopy_temp)
node04054=(OME_SCALAR)node03632-node04052;

//=-=-=-=-=-=-=-=-=-= node04064 (sensible_heat_flux)
node04064=(OME_SCALAR)elementRaw(node03821,node03988_i+1,1)*29.17*(node04054-node03604);

//=-=-=-=-=-=-=-=-=-= node04056 (evaporation)
node04056=(OME_SCALAR)(elementRaw(node03973,node03988_i+1,1)-node04064-node04043)/2257.0;

//=-=-=-=-=-=-=-=-=-= node04046 (Air_temp)
node04046=(OME_SCALAR)elementRaw(node03823,node03988_i+1,1)+elementRaw(node03817,node03988_i+1,1)*node04054;

if(__OME__posgreatest_0_second < (*root.GetValueArchive())[node04060.GetModelIndex()+node03988_i+15*(node03644_i)]) { __OME__posgreatest_0=node03988_i+1; __OME__posgreatest_0_second=(*root.GetValueArchive())[node04060.GetModelIndex()+node03988_i+15*(node03644_i)];}
}
node03988.SetCurrentInstance(0);

OME_SCALAR __OME__sum_13;
__OME__sum_13= 0.0;
OME_SCALAR __OME__sum_12;
__OME__sum_12= 0.0;
OME_SCALAR __OME__sum_10;
__OME__sum_10= 0.0;
OME_SCALAR __OME__sum_9;
__OME__sum_9= 0.0;
for(size_t __OME__tmpLoop_24_i=0;__OME__tmpLoop_24_i<15;++__OME__tmpLoop_24_i)
{

__OME__sum_9+=(*root.GetValueArchive())[node04041.GetModelIndex()+__OME__tmpLoop_24_i+15*(node03644_i)];
__OME__sum_10+=(*root.GetValueArchive())[node04030.GetModelIndex()+__OME__tmpLoop_24_i+15*(node03644_i)];
__OME__sum_12+=(*root.GetValueArchive())[node04054.GetModelIndex()+__OME__tmpLoop_24_i+15*(node03644_i)]*(*root.GetValueArchive())[node04060.GetModelIndex()+__OME__tmpLoop_24_i+15*(node03644_i)];
__OME__sum_13+=(*root.GetValueArchive())[node04046.GetModelIndex()+__OME__tmpLoop_24_i+15*(node03644_i)]*(*root.GetValueArchive())[node04060.GetModelIndex()+__OME__tmpLoop_24_i+15*(node03644_i)];
}
//=-=-=-=-=-=-=-=-=-= node03975 (Average_air_temperatures)
node03975=(OME_SCALAR)__OME__sum_13;

OME_SCALAR __OME__sum_11;
__OME__sum_11= 0.0;
for(size_t __OME__tmpLoop_26_i=0;__OME__tmpLoop_26_i<1;++__OME__tmpLoop_26_i)
{
node00017.SetCurrentInstance(__OME__tmpLoop_26_i);
node03967.SetCurrentInstance(__OME__tmpLoop_26_i);

OME_SCALAR __OME__firsttrue_3;
__OME__firsttrue_3=0;
for(size_t __OME__tmpLoop_29_i=0;__OME__tmpLoop_29_i<16;++__OME__tmpLoop_29_i)
{

if(!__OME__firsttrue_3 && node04023<(*root.GetValueArchive())[node03674.GetModelIndex()+__OME__tmpLoop_29_i]) __OME__firsttrue_3=__OME__tmpLoop_29_i+1;
}
//=-=-=-=-=-=-=-=-=-= node04024 (Wealth_position)
node04024=(OME_SCALAR)__OME__firsttrue_3;

__OME__sum_11+=(*root.GetValueArchive())[node04150.GetModelIndex()];
//=-=-=-=-=-=-=-=-=-= arc01739 (reset_to_0)
arc01739=(OME_SCALAR)(node03638==273.0 ? (OME_SCALAR)node00052-1.0 : (OME_SCALAR)0.0);

//=-=-=-=-=-=-=-=-=-= node00021 (Heat_wave_temperatures)
node00021=(OME_SCALAR)(node03975>node00045 ? (OME_SCALAR)1.0 : (OME_SCALAR)0.0);

//=-=-=-=-=-=-=-=-=-= arc01510 (add_time)
arc01510=(OME_SCALAR)node00021;

//=-=-=-=-=-=-=-=-=-= arc01406 (reset_intensity)
arc01406=(OME_SCALAR)(1.0-node00021)*node00024;

//=-=-=-=-=-=-=-=-=-= arc01203 (Add_High_tempreatures)
arc01203=(OME_SCALAR)node00021*OMEMax((0.0),(node03975-node00045));

//=-=-=-=-=-=-=-=-=-= node00047 (Heat_wave_starts)
node00047=(OME_SCALAR)(last(TestModelBm,node00021)==0.0&&node00021==1.0 ? (OME_SCALAR)node03638-121.0 : (OME_SCALAR)366.0);

//=-=-=-=-=-=-=-=-=-= arc01511 (end_of_period)
arc01511=(OME_SCALAR)(1.0-node00021)*node00030;

//=-=-=-=-=-=-=-=-=-= arc01738 (Next_wave)
arc01738=(OME_SCALAR)(arc01511>0.0 ? (OME_SCALAR)1.0 : (OME_SCALAR)0.0);

//=-=-=-=-=-=-=-=-=-= node00034 (_Heat_effect)
node00034=(OME_SCALAR)OMEMax((0.0),(node03975-node00045))/5.2;

}
node00017.SetCurrentInstance(0);
node03967.SetCurrentInstance(0);

OME_SCALAR __OME__sum_23[11];
size_t __OME__sum_23_dims[]={11};
OME_SCALAR __OME__sum_22[2][11];
size_t __OME__sum_22_dims[]={2,11};
OME_SCALAR __OME__sum_20[11];
size_t __OME__sum_20_dims[]={11};
OME_SCALAR __OME__sum_19[2][11];
size_t __OME__sum_19_dims[]={2,11};
OME_SCALAR __OME__sum_17[16][11];
size_t __OME__sum_17_dims[]={16,11};
OME_SCALAR __OME__sum_15[11];
size_t __OME__sum_15_dims[]={11};
OME_SCALAR __OME__sum_14[224][11];
size_t __OME__sum_14_dims[]={224,11};
for(size_t __OME__iniLoop_6_i=0;__OME__iniLoop_6_i<11;++__OME__iniLoop_6_i)
{

for(size_t __OME__iniLoop_8_i=0;__OME__iniLoop_8_i<2;++__OME__iniLoop_8_i)
{

__OME__sum_22[__OME__iniLoop_8_i][__OME__iniLoop_6_i]= 0.0;
__OME__sum_19[__OME__iniLoop_8_i][__OME__iniLoop_6_i]= 0.0;
}
for(size_t __OME__iniLoop_13_i=0;__OME__iniLoop_13_i<16;++__OME__iniLoop_13_i)
{

__OME__sum_17[__OME__iniLoop_13_i][__OME__iniLoop_6_i]= 0.0;
}
for(size_t __OME__iniLoop_16_i=0;__OME__iniLoop_16_i<224;++__OME__iniLoop_16_i)
{

__OME__sum_14[__OME__iniLoop_16_i][__OME__iniLoop_6_i]= 0.0;
}
__OME__sum_23[__OME__iniLoop_6_i]= 0.0;
__OME__sum_20[__OME__iniLoop_6_i]= 0.0;
__OME__sum_15[__OME__iniLoop_6_i]= 0.0;
}
OME_SCALAR __OME__tempArray_15[]={node00036,node00034};
size_t __OME__tempArray_15_dims[]={2};
//=-=-=-=-=-=-=-=-=-= node03965 (Heat_Wave)
node03965=(OME_SCALAR)element((OME_SCALAR*)__OME__tempArray_15,__OME__tempArray_15_dims,node00059,1);

OME_SCALAR __OME__tempArray_16[]={node03965,0.0,node03954,0.0,0.0,0.0,0.0,0.0,0.0,1.0};
size_t __OME__tempArray_16_dims[]={10};
for(size_t __OME__tmpLoop_30_i=0;__OME__tmpLoop_30_i<10;++__OME__tmpLoop_30_i)
{

//=-=-=-=-=-=-=-=-=-= node03961 (Hazards)
(*root.GetValueArchive())[node03961.GetModelIndex()+__OME__tmpLoop_30_i+10*(node03644_i)]=(OME_SCALAR)__OME__tempArray_16[__OME__tmpLoop_30_i];

}
for(size_t __OME__tmpLoop_31_i=0;__OME__tmpLoop_31_i<10;++__OME__tmpLoop_31_i)
{

for(size_t __OME__tmpLoop_32_i=0;__OME__tmpLoop_32_i<224;++__OME__tmpLoop_32_i)
{

for(size_t __OME__tmpLoop_33_i=0;__OME__tmpLoop_33_i<11;++__OME__tmpLoop_33_i)
{

__OME__sum_14[__OME__tmpLoop_32_i][__OME__tmpLoop_33_i]+=(*root.GetValueArchive())[node03961.GetModelIndex()+__OME__tmpLoop_31_i+10*(node03644_i)]*(*root.GetValueArchive())[node03746.GetModelIndex()+__OME__tmpLoop_33_i+11*(__OME__tmpLoop_32_i+224*(__OME__tmpLoop_31_i))];
}
}
for(size_t __OME__tmpLoop_37_i=0;__OME__tmpLoop_37_i<16;++__OME__tmpLoop_37_i)
{

for(size_t __OME__tmpLoop_38_i=0;__OME__tmpLoop_38_i<11;++__OME__tmpLoop_38_i)
{

__OME__sum_17[__OME__tmpLoop_37_i][__OME__tmpLoop_38_i]+=(*root.GetValueArchive())[node03961.GetModelIndex()+__OME__tmpLoop_31_i+10*(node03644_i)]*(*root.GetValueArchive())[node03759.GetModelIndex()+__OME__tmpLoop_38_i+11*(__OME__tmpLoop_37_i+16*(__OME__tmpLoop_31_i))];
}
}
for(size_t __OME__tmpLoop_40_i=0;__OME__tmpLoop_40_i<2;++__OME__tmpLoop_40_i)
{

for(size_t __OME__tmpLoop_41_i=0;__OME__tmpLoop_41_i<11;++__OME__tmpLoop_41_i)
{

__OME__sum_19[__OME__tmpLoop_40_i][__OME__tmpLoop_41_i]+=(*root.GetValueArchive())[node03961.GetModelIndex()+__OME__tmpLoop_31_i+10*(node03644_i)]*(*root.GetValueArchive())[node03755.GetModelIndex()+__OME__tmpLoop_41_i+11*(__OME__tmpLoop_40_i+2*(__OME__tmpLoop_31_i))];
__OME__sum_22[__OME__tmpLoop_40_i][__OME__tmpLoop_41_i]+=(*root.GetValueArchive())[node03961.GetModelIndex()+__OME__tmpLoop_31_i+10*(node03644_i)]*(*root.GetValueArchive())[node03751.GetModelIndex()+__OME__tmpLoop_41_i+11*(__OME__tmpLoop_40_i+2*(__OME__tmpLoop_31_i))];
}
}
}
for(size_t __OME__tmpLoop_34_i=0;__OME__tmpLoop_34_i<224;++__OME__tmpLoop_34_i)
{

for(size_t __OME__tmpLoop_35_i=0;__OME__tmpLoop_35_i<11;++__OME__tmpLoop_35_i)
{

__OME__sum_15[__OME__tmpLoop_35_i]+=(*root.GetValueArchive())[node04019.GetModelIndex()+__OME__tmpLoop_34_i+224*(node03644_i)]*__OME__sum_14[__OME__tmpLoop_34_i][__OME__tmpLoop_35_i];
}
__OME__sum_8+=__OME__sum_7[__OME__tmpLoop_34_i];
}
for(size_t __OME__tmpLoop_42_i=0;__OME__tmpLoop_42_i<2;++__OME__tmpLoop_42_i)
{

for(size_t __OME__tmpLoop_43_i=0;__OME__tmpLoop_43_i<11;++__OME__tmpLoop_43_i)
{

__OME__sum_20[__OME__tmpLoop_43_i]+=(*root.GetValueArchive())[node04020.GetModelIndex()+__OME__tmpLoop_42_i+2*(node03644_i)]*__OME__sum_19[__OME__tmpLoop_42_i][__OME__tmpLoop_43_i];
__OME__sum_23[__OME__tmpLoop_43_i]+=(*root.GetValueArchive())[node04021.GetModelIndex()+__OME__tmpLoop_42_i+2*(node03644_i)]*__OME__sum_22[__OME__tmpLoop_42_i][__OME__tmpLoop_43_i];
}
}
OME_SCALAR __OME__sum_24;
__OME__sum_24= 0.0;
OME_SCALAR __OME__sum_21;
__OME__sum_21= 0.0;
OME_SCALAR __OME__sum_18;
__OME__sum_18= 0.0;
OME_SCALAR __OME__sum_16;
__OME__sum_16= 0.0;
for(size_t __OME__tmpLoop_49_i=0;__OME__tmpLoop_49_i<11;++__OME__tmpLoop_49_i)
{

__OME__sum_16+=__OME__sum_15[__OME__tmpLoop_49_i];
__OME__sum_18+=element((OME_SCALAR*)__OME__sum_17,__OME__sum_17_dims,node04024,2)[__OME__tmpLoop_49_i];
__OME__sum_21+=__OME__sum_20[__OME__tmpLoop_49_i];
__OME__sum_24+=__OME__sum_23[__OME__tmpLoop_49_i];
}
//=-=-=-=-=-=-=-=-=-= node03963 (Est_Morbidity)
node03963=(OME_SCALAR)0.05*__OME__sum_16+__OME__sum_18+__OME__sum_21+__OME__sum_24;

OME_SCALAR __OME__sum_25;
__OME__sum_25= 0.0;
__OME__sum_25+=__OME__sum_18;
//=-=-=-=-=-=-=-=-=-= node00079 (Mortality_rt)
node00079=(OME_SCALAR)__OME__sum_16+__OME__sum_25+__OME__sum_21+__OME__sum_24;

//=-=-=-=-=-=-=-=-=-= arc04360 (Montly_Sum)
arc04360=(OME_SCALAR)(__OME__sum_11==1.0 ? (OME_SCALAR)node03613 : (OME_SCALAR)0.0);

//=-=-=-=-=-=-=-=-=-= node00230 (DU_Evap_contribution)
node00230=(OME_SCALAR)node03970/node00228*arc04360;

//=-=-=-=-=-=-=-=-=-= node00077 (Num_Mortality)
node00077=(OME_SCALAR)node00034*node00062;

//=-=-=-=-=-=-=-=-=-= node03958 (NDVI)
node03958=(OME_SCALAR)__OME__sum_9;

//=-=-=-=-=-=-=-=-=-= node03934 (Tree_Cover)
node03934=(OME_SCALAR)__OME__sum_10/node03970;

//=-=-=-=-=-=-=-=-=-= node03979 (Av_Can_T)
node03979=(OME_SCALAR)__OME__sum_12;

//=-=-=-=-=-=-=-=-=-= node04000 (Population_modifier)
node04000=(OME_SCALAR)node03927/node03602;

//=-=-=-=-=-=-=-=-=-= node03950 (EMS)
node03950=(OME_SCALAR)OMEMax((0.0),(-8.262408+0.3614094*node03979))*node04000;

//=-=-=-=-=-=-=-=-=-= node03952 (Complaints)
node03952=(OME_SCALAR)OMEMax((0.0),(-8.031008+0.5187364*node03979))*node04000;

//=-=-=-=-=-=-=-=-=-= node03977 (Forest_T)
node03977=(OME_SCALAR)element(node04054,7.0,1);

//=-=-=-=-=-=-=-=-=-= node03971 (dominant_lu)
node03971=(OME_SCALAR)__OME__posgreatest_0;

//=-=-=-=-=-=-=-=-=-= node03990 (Bare_Ground_T)
node03990=(OME_SCALAR)element(node04054,6.0,1);

//=-=-=-=-=-=-=-=-=-= node03862 (Non_white_pop)
node03862=(OME_SCALAR)__OME__sum_6/__OME__sum_8;

//=-=-=-=-=-=-=-=-=-= node03834 (Children)
node03834=(OME_SCALAR)__OME__sum_5;

//=-=-=-=-=-=-=-=-=-= node03836 (Elderly)
node03836=(OME_SCALAR)__OME__sum_4;

//=-=-=-=-=-=-=-=-=-= node03830 (Empl)
node03830=(OME_SCALAR)elementRaw(node04015,node03644_i+1,1);

//=-=-=-=-=-=-=-=-=-= node03826 (Wealth)
node03826=(OME_SCALAR)elementRaw(node04023,node03644_i+1,1);

OME_SCALAR __OME__tempArray_10[]={node03834,node03836,node03862,node03826,node03830};
size_t __OME__tempArray_10_dims[]={5};
//=-=-=-=-=-=-=-=-=-= node03853 (Social_Fabric_Display)
node03853=(OME_SCALAR)element((OME_SCALAR*)__OME__tempArray_10,__OME__tempArray_10_dims,node03695,1);

//=-=-=-=-=-=-=-=-=-= node03828 (Rural)
node03828=(OME_SCALAR)elementRaw(node04013,node03644_i+1,1);

//=-=-=-=-=-=-=-=-=-= node03844 (Complaints_Index)
node03844=(OME_SCALAR)element(node03896,2.0,1);

//=-=-=-=-=-=-=-=-=-= node03848 (EMS_Index)
node03848=(OME_SCALAR)element(node03896,1.0,1);

//=-=-=-=-=-=-=-=-=-= node03872 (blgr_ID)
node03872=(OME_SCALAR)element(node03641,node03644_i+1,1);

//=-=-=-=-=-=-=-=-=-= node03983 (Land_Cover)
node03983=(OME_SCALAR)element(node04060,node03637,1);

//=-=-=-=-=-=-=-=-=-= node00073 (Cat_Mortality_rt)
node00073=(OME_SCALAR)node00048*(0.0374+((node00052>1.0 ? (OME_SCALAR)0.0504 : (OME_SCALAR)0.0265))+node00046/1.8*0.249+node00030*0.038);

}
node03644.SetCurrentInstance(0);
node03642.SetCurrentInstance(0);

//=-=-=-=-=-=-=-=-=-= node03669 (month)
node03669=(OME_SCALAR)__OME__firsttrue_0;

for(size_t node03700_i=0;node03700_i<1;++node03700_i)
{
node03700.SetCurrentInstance(node03700_i);

//=-=-=-=-=-=-=-=-=-= node04105 (Start_of_GCC)
node04105.EvalWithVal(node03622>27.0);

node03700.GetCurrentInstanceRef()->ApplyMortality();
if (node03700.GetCurrentInstanceActive())
{
//=-=-=-=-=-=-=-=-=-= node04108 (Year)
node04108=(OME_SCALAR)node03622-27.0;

//=-=-=-=-=-=-=-=-=-= node04112 (GCC_M_Relative_Humidity)
node04112=(OME_SCALAR)element(elementRaw(node03766,node03669,2),node04108);

//=-=-=-=-=-=-=-=-=-= node04093 (GCC_M_Temp)
node04093=(OME_SCALAR)element(elementRaw(node03767,node03669,2),node04108)-273.15;

//=-=-=-=-=-=-=-=-=-= node04094 (Precip_sc)
node04094=(OME_SCALAR)element(elementRaw(node03765,node03669,2),node04108);

//=-=-=-=-=-=-=-=-=-= node04113 (GCC_Wind_speeds)
node04113=(OME_SCALAR)element(elementRaw(node03768,node03669,2),node04108);

} //node03700
}
node03700.SetCurrentInstance(0);

OME_SCALAR __OME__sum_28;
__OME__sum_28= 0.0;
OME_SCALAR __OME__sum_27;
__OME__sum_27= 0.0;
OME_SCALAR __OME__sum_26;
__OME__sum_26= 0.0;
for(size_t __OME__tmpLoop_53_i=0;__OME__tmpLoop_53_i<696;++__OME__tmpLoop_53_i)
{

for(size_t __OME__tmpLoop_57_i=0;__OME__tmpLoop_57_i<15;++__OME__tmpLoop_57_i)
{

__OME__sum_29[__OME__tmpLoop_57_i]+=(*root.GetValueArchive())[node04046.GetModelIndex()+__OME__tmpLoop_57_i+15*(__OME__tmpLoop_53_i)];
__OME__sum_30[__OME__tmpLoop_57_i]+=(*root.GetValueArchive())[node04054.GetModelIndex()+__OME__tmpLoop_57_i+15*(__OME__tmpLoop_53_i)];
__OME__sum_31[__OME__tmpLoop_57_i]+=(*root.GetValueArchive())[node04052.GetModelIndex()+__OME__tmpLoop_57_i+15*(__OME__tmpLoop_53_i)];
}
__OME__sum_26+=(*root.GetValueArchive())[node03975.GetModelIndex()+__OME__tmpLoop_53_i]*(*root.GetValueArchive())[node03970.GetModelIndex()+__OME__tmpLoop_53_i];
__OME__sum_27+=(*root.GetValueArchive())[node03977.GetModelIndex()+__OME__tmpLoop_53_i]*(*root.GetValueArchive())[node03970.GetModelIndex()+__OME__tmpLoop_53_i];
__OME__sum_28+=(*root.GetValueArchive())[node03990.GetModelIndex()+__OME__tmpLoop_53_i]*(*root.GetValueArchive())[node03970.GetModelIndex()+__OME__tmpLoop_53_i];
}
for(size_t __OME__tmpLoop_58_i=0;__OME__tmpLoop_58_i<15;++__OME__tmpLoop_58_i)
{

//=-=-=-=-=-=-=-=-=-= node03677 (LandUse_air_T)
(*root.GetValueArchive())[node03677.GetModelIndex()+__OME__tmpLoop_58_i]=(OME_SCALAR)__OME__sum_29[__OME__tmpLoop_58_i]/696.0;

//=-=-=-=-=-=-=-=-=-= node03696 (LandUse_Canopy_T)
(*root.GetValueArchive())[node03696.GetModelIndex()+__OME__tmpLoop_58_i]=(OME_SCALAR)__OME__sum_30[__OME__tmpLoop_58_i]/696.0;

//=-=-=-=-=-=-=-=-=-= node03728 (Temp_red)
(*root.GetValueArchive())[node03728.GetModelIndex()+__OME__tmpLoop_58_i]=(OME_SCALAR)__OME__sum_31[__OME__tmpLoop_58_i]/696.0;

}
OME_SCALAR __OME__sum_33;
__OME__sum_33= 0.0;
OME_SCALAR __OME__sum_32;
__OME__sum_32= 0.0;
for(size_t __OME__tmpLoop_65_i=0;__OME__tmpLoop_65_i<1;++__OME__tmpLoop_65_i)
{
node03717.SetCurrentInstance(__OME__tmpLoop_65_i);
node03730.SetCurrentInstance(__OME__tmpLoop_65_i);

__OME__sum_32+=(*root.GetValueArchive())[node04094.GetModelIndex()];
__OME__sum_33+=(*root.GetValueArchive())[node04113.GetModelIndex()];
OME_SCALAR __OME__firsttrue_4;
__OME__firsttrue_4=0;
OME_SCALAR __OME__sum_36;
__OME__sum_36= 0.0;
node03717.GetCurrentInstanceRef()->ApplyMortality();
if (node03717.GetCurrentInstanceActive())
{
for(size_t __OME__tmpLoop_69_i=0;__OME__tmpLoop_69_i<696;++__OME__tmpLoop_69_i)
{

__OME__sum_36+=(*root.GetValueArchive())[node00230.GetModelIndex()+__OME__tmpLoop_69_i];
}
OME_SCALAR __OME__tempArray_19[]={31.0,59.0,90.0,120.0,151.0,181.0,212.0,243.0,273.0,304.0,334.0,365.0};
size_t __OME__tempArray_19_dims[]={12};
for(size_t __OME__tmpLoop_70_i=0;__OME__tmpLoop_70_i<12;++__OME__tmpLoop_70_i)
{

if(!__OME__firsttrue_4 && node03638==__OME__tempArray_19[__OME__tmpLoop_70_i]) __OME__firsttrue_4=__OME__tmpLoop_70_i+1;
}
//=-=-=-=-=-=-=-=-=-= node04138 (month)
node04138=(OME_SCALAR)__OME__firsttrue_4;

//=-=-=-=-=-=-=-=-=-= node04135 (Measured_Values)
node04135=(OME_SCALAR)element(elementRaw(node03734,node03622,2),node04138);

//=-=-=-=-=-=-=-=-=-= node04147 (Montly_Evaporation)
node04147=(OME_SCALAR)__OME__sum_36;

//=-=-=-=-=-=-=-=-=-= node04140 (year)
node04140=(OME_SCALAR)node03622+1972.0;

} //node03717
OME_SCALAR __OME__sum_39;
__OME__sum_39= 0.0;
OME_SCALAR __OME__sum_38;
__OME__sum_38= 0.0;
//=-=-=-=-=-=-=-=-=-= node04153 (cond1)
node04153.EvalWithVal(node03622==39.0&&node03638>152.0&&node03638<274.0);

node03730.GetCurrentInstanceRef()->ApplyMortality();
if (node03730.GetCurrentInstanceActive())
{
for(size_t __OME__tmpLoop_73_i=0;__OME__tmpLoop_73_i<696;++__OME__tmpLoop_73_i)
{

__OME__sum_38+=(*root.GetValueArchive())[node03950.GetModelIndex()+__OME__tmpLoop_73_i];
__OME__sum_39+=(*root.GetValueArchive())[node03952.GetModelIndex()+__OME__tmpLoop_73_i];
}
//=-=-=-=-=-=-=-=-=-= node04157 (EMS_model)
node04157=(OME_SCALAR)__OME__sum_38;

//=-=-=-=-=-=-=-=-=-= node04163 (EMS_data)
node04163=(OME_SCALAR)elementRaw(node04173,node03638-152.0,1);

//=-=-=-=-=-=-=-=-=-= node04169 (EMS_sq_Err)
node04169=(OME_SCALAR)OMECFuncs::pow((node04157-node04163),2.0);

//=-=-=-=-=-=-=-=-=-= node04159 (Complaints_model)
node04159=(OME_SCALAR)__OME__sum_39;

//=-=-=-=-=-=-=-=-=-= node04165 (Complaints)
node04165=(OME_SCALAR)elementRaw(node04174,node03638-152.0,1);

//=-=-=-=-=-=-=-=-=-= node04171 (Complaints__sq_err)
node04171=(OME_SCALAR)OMECFuncs::pow((node04165-node04159),2.0);

} //node03730
node03717.GetCurrentInstanceRef()->ApplyMortality();
if (node03717.GetCurrentInstanceActive())
{
OME_SCALAR __OME__tempArray_20[]={node04135,node04147,node04140};
size_t __OME__tempArray_20_dims[]={3};
for(size_t __OME__tmpLoop_80_i=0;__OME__tmpLoop_80_i<3;++__OME__tmpLoop_80_i)
{

//=-=-=-=-=-=-=-=-=-= node04142 (Comparison)
(*root.GetValueArchive())[node04142.GetModelIndex()+__OME__tmpLoop_80_i]=(OME_SCALAR)__OME__tempArray_20[__OME__tmpLoop_80_i];

}
} //node03717
}
node03717.SetCurrentInstance(0);
node03730.SetCurrentInstance(0);

//=-=-=-=-=-=-=-=-=-= node03668 (Windspeed)
node03668=(OME_SCALAR)(currenttime(TestModelBm)==0.0 ? (OME_SCALAR)0.0 : (OME_SCALAR)((node03612==0.0 ? (OME_SCALAR)element(element(node03781,node03638,2),node03622) : (OME_SCALAR)__OME__sum_33)));

//=-=-=-=-=-=-=-=-=-= node00060 (Mortality_Choice)
node00060.Evaluate(TestModelTime,TestModelBm);

for(size_t node03644_i=0;node03644_i<696;++node03644_i)
{
node03644.SetCurrentInstance(node03644_i);
node03642.SetCurrentInstance(node03644_i);

OME_SCALAR __OME__sum_35;
__OME__sum_35= 0.0;
for(size_t __OME__tmpLoop_68_i=0;__OME__tmpLoop_68_i<15;++__OME__tmpLoop_68_i)
{

__OME__sum_35+=(*root.GetValueArchive())[node04034.GetModelIndex()+__OME__tmpLoop_68_i+15*(node03644_i)];
}
//=-=-=-=-=-=-=-=-=-= node04004 (Windspeed)
node04004=(OME_SCALAR)node03668-__OME__sum_35;

//=-=-=-=-=-=-=-=-=-= node03939 (Air_stagnation)
node03939=(OME_SCALAR)(node04004<13.0 ? (OME_SCALAR)1.0 : (OME_SCALAR)0.0);

for(size_t node04002_i=0;node04002_i<6;++node04002_i)
{
node04002.SetCurrentInstance(node04002_i);

OME_SCALAR __OME__sum_34;
__OME__sum_34= 0.0;
for(size_t __OME__tmpLoop_67_i=0;__OME__tmpLoop_67_i<15;++__OME__tmpLoop_67_i)
{

__OME__sum_34+=(*root.GetValueArchive())[node04030.GetModelIndex()+__OME__tmpLoop_67_i+15*(node03644_i)];
}
//=-=-=-=-=-=-=-=-=-= node04080 (Pollutant_removal_potential)
node04080=(OME_SCALAR)__OME__sum_34*elementRaw(node03788,node04002_i+1,1);

//=-=-=-=-=-=-=-=-=-= arc04194 (Pollutant_removal)
arc04194=(OME_SCALAR)OMEMax((0.0),(OMEMin((node04081),(node04080))));

//=-=-=-=-=-=-=-=-=-= node04071 (diffusion_rt)
node04071=(OME_SCALAR)(elementRaw(node03794,node04002_i+1,1)==1.0 ? (OME_SCALAR)(1.0-node03939)*elementRaw(node03800,node04002_i+1,1) : (OME_SCALAR)elementRaw(node03800,node04002_i+1,1));

//=-=-=-=-=-=-=-=-=-= arc04201 (Pollutant_production)
arc04201=(OME_SCALAR)(elementRaw(node03797,node04002_i+1,1)-node04081)*node04071;

}
node04002.SetCurrentInstance(0);

for(size_t node03847_i=0;node03847_i<5;++node03847_i)
{
node03847.SetCurrentInstance(node03847_i);

OME_SCALAR __OME__tempArray_18[]={elementRaw(node03950,node03644_i+1,1),elementRaw(node03952,node03644_i+1,1),elementRaw(node00079,node03644_i+1,1),elementRaw(node00077,node03644_i+1,1),elementRaw(node00073,node03644_i+1,1)};
size_t __OME__tempArray_18_dims[]={5};
//=-=-=-=-=-=-=-=-=-= node03904 (Morbidity)
node03904=(OME_SCALAR)element((OME_SCALAR*)__OME__tempArray_18,__OME__tempArray_18_dims,node03847_i+1,1);

}
node03847.SetCurrentInstance(0);

OME_SCALAR __OME__tempArray_17[]={node00079,node00077,node00073};
size_t __OME__tempArray_17_dims[]={3};
//=-=-=-=-=-=-=-=-=-= node00074 (Mortality)
node00074=(OME_SCALAR)element((OME_SCALAR*)__OME__tempArray_17,__OME__tempArray_17_dims,node00060,1);

}
node03644.SetCurrentInstance(0);
node03642.SetCurrentInstance(0);

//=-=-=-=-=-=-=-=-=-= node03694 (Choose_Heat_Mortality_Index)
node03694.Evaluate(TestModelTime,TestModelBm);

for(size_t __OME__tmpLoop_71_i=0;__OME__tmpLoop_71_i<696;++__OME__tmpLoop_71_i)
{
node03644.SetCurrentInstance(__OME__tmpLoop_71_i);
node03642.SetCurrentInstance(__OME__tmpLoop_71_i);

for(size_t __OME__tmpLoop_72_i=0;__OME__tmpLoop_72_i<5;++__OME__tmpLoop_72_i)
{
node03847.SetCurrentInstance(__OME__tmpLoop_72_i);

__OME__sum_37[__OME__tmpLoop_72_i]+=(*root.GetValueArchive())[node03904.GetModelIndex()+__OME__tmpLoop_72_i+5*(__OME__tmpLoop_71_i)];
//=-=-=-=-=-=-=-=-=-= arc03930 (add_mortalities_rt)
arc03930=(OME_SCALAR)node03904-((node03638==365.0 ? (OME_SCALAR)node03896 : (OME_SCALAR)0.0));

}
node03847.SetCurrentInstance(0);

OME_SCALAR __OME__sum_40;
__OME__sum_40= 0.0;
for(size_t __OME__tmpLoop_75_i=0;__OME__tmpLoop_75_i<15;++__OME__tmpLoop_75_i)
{

__OME__sum_40+=(*root.GetValueArchive())[node04056.GetModelIndex()+__OME__tmpLoop_75_i+15*(__OME__tmpLoop_71_i)]*(*root.GetValueArchive())[node04060.GetModelIndex()+__OME__tmpLoop_75_i+15*(__OME__tmpLoop_71_i)];
}

OME_SCALAR __OME__sum_41;
__OME__sum_41= 0.0;
for(size_t __OME__tmpLoop_76_i=0;__OME__tmpLoop_76_i<696;++__OME__tmpLoop_76_i)
{

__OME__sum_41+=(*root.GetValueArchive())[node03975.GetModelIndex()+__OME__tmpLoop_76_i];
}

//=-=-=-=-=-=-=-=-=-= node03869 (Air_temperatures_deviation)
node03869=(OME_SCALAR)elementRaw(node03975,__OME__tmpLoop_71_i+1,1)-__OME__sum_41/node03644.GetNumInstances();

//=-=-=-=-=-=-=-=-=-= node03929 (DU_Evap)
node03929=(OME_SCALAR)__OME__sum_40;

//=-=-=-=-=-=-=-=-=-= arc04227 (Add_average_daily_evap)
arc04227=(OME_SCALAR)node03929*3.401568;

//=-=-=-=-=-=-=-=-=-= node03893 (Bare_ground_T)
node03893=(OME_SCALAR)elementRaw(node03990,__OME__tmpLoop_71_i+1,1);

//=-=-=-=-=-=-=-=-=-= node03883 (Landcover)
node03883=(OME_SCALAR)elementRaw(node03983,__OME__tmpLoop_71_i+1,1);

//=-=-=-=-=-=-=-=-=-= node03880 (Surface_Temperature)
node03880=(OME_SCALAR)elementRaw(node03979,__OME__tmpLoop_71_i+1,1);

//=-=-=-=-=-=-=-=-=-= node03877 (Dominant_LU)
node03877=(OME_SCALAR)elementRaw(node03971,__OME__tmpLoop_71_i+1,1);

//=-=-=-=-=-=-=-=-=-= node03841 (Water_temperature_reduction)
node03841=(OME_SCALAR)element(elementRaw(node04052,__OME__tmpLoop_71_i+1,2),1.0);

//=-=-=-=-=-=-=-=-=-= node03851 (Morbidity_est)
node03851=(OME_SCALAR)element(node03904,node03694,1);

//=-=-=-=-=-=-=-=-=-= node03856 (Tree_cover_ratio)
node03856=(OME_SCALAR)elementRaw(node03934,__OME__tmpLoop_71_i+1,1);

//=-=-=-=-=-=-=-=-=-= node03863 (NDVI)
node03863=(OME_SCALAR)elementRaw(node03958,__OME__tmpLoop_71_i+1,1);

//=-=-=-=-=-=-=-=-=-= node03871 (Heat_Wave)
node03871=(OME_SCALAR)elementRaw(node03965,__OME__tmpLoop_71_i+1,1);

}
node03644.SetCurrentInstance(0);
node03642.SetCurrentInstance(0);

for(size_t node03611_i=0;node03611_i<6;++node03611_i)
{
node03611.SetCurrentInstance(node03611_i);

OME_SCALAR __OME__sum_42[6];
size_t __OME__sum_42_dims[]={6};
for(size_t __OME__iniLoop_17_i=0;__OME__iniLoop_17_i<6;++__OME__iniLoop_17_i)
{

__OME__sum_42[__OME__iniLoop_17_i]= 0.0;
}
for(size_t __OME__tmpLoop_77_i=0;__OME__tmpLoop_77_i<696;++__OME__tmpLoop_77_i)
{

for(size_t __OME__tmpLoop_78_i=0;__OME__tmpLoop_78_i<6;++__OME__tmpLoop_78_i)
{

__OME__sum_42[__OME__tmpLoop_78_i]+=(*root.GetValueArchive())[arc04194.GetModelIndex()+__OME__tmpLoop_78_i+6*(__OME__tmpLoop_77_i)];
}
}
OME_SCALAR __OME__tmp_lst_0[6]={};
size_t __OME__tmp_lst_0_dims[]={6};
for(size_t __OME__tmpLoop_79_i=0;__OME__tmpLoop_79_i<6;++__OME__tmpLoop_79_i)
{

__OME__tmp_lst_0[__OME__tmpLoop_79_i]=__OME__sum_42[__OME__tmpLoop_79_i]*node03790/2000.0;
}
//=-=-=-=-=-=-=-=-=-= node03792 (Total_pollutant_Removal_Value)
node03792=(OME_SCALAR)element((OME_SCALAR*)__OME__tmp_lst_0,__OME__tmp_lst_0_dims,node03611_i+1,1);

}
node03611.SetCurrentInstance(0);

//=-=-=-=-=-=-=-=-=-= node03720 (EMS_modeled)
node03720=(OME_SCALAR)element((OME_SCALAR*)__OME__sum_37,__OME__sum_37_dims,1.0,1);

//=-=-=-=-=-=-=-=-=-= node03723 (EMS_cat)
node03723=(OME_SCALAR)element((OME_SCALAR*)__OME__sum_37,__OME__sum_37_dims,3.0,1);

//=-=-=-=-=-=-=-=-=-= node03725 (EMS_num)
node03725=(OME_SCALAR)element((OME_SCALAR*)__OME__sum_37,__OME__sum_37_dims,2.0,1);

//=-=-=-=-=-=-=-=-=-= node03704 (Chance_Position)
node03704=(OME_SCALAR)__OME__firsttrue_2;

//=-=-=-=-=-=-=-=-=-= node03693 (GCC_Rainfall)
node03693=(OME_SCALAR)__OME__sum_32*element(element(node03706,node03704,2),node03669)*3.0;

//=-=-=-=-=-=-=-=-=-= node03707 (Rain)
node03707=(OME_SCALAR)(currenttime(TestModelBm)==0.0 ? (OME_SCALAR)0.0 : (OME_SCALAR)((node03612==0.0 ? (OME_SCALAR)element(element(node03786,node03638,2),node03622) : (OME_SCALAR)node03693)));

//=-=-=-=-=-=-=-=-=-= arc04266 (add_precip)
arc04266=(OME_SCALAR)node03707;

//=-=-=-=-=-=-=-=-=-= arc04268 (remove_precip)
arc04268=(OME_SCALAR)var_delay((arc04266),((currenttime(TestModelBm)<node03682 ? (OME_SCALAR)1.0 : (OME_SCALAR)node03682)));

//=-=-=-=-=-=-=-=-=-= node03603 (Avr_T)
node03603=(OME_SCALAR)__OME__sum_26/__OME__sum_2;

//=-=-=-=-=-=-=-=-=-= node03617 (Forest__Temperatures)
node03617=(OME_SCALAR)__OME__sum_27/__OME__sum_2;

//=-=-=-=-=-=-=-=-=-= node03619 (Bare_ground_Temperatures)
node03619=(OME_SCALAR)__OME__sum_28/__OME__sum_2;

//=-=-=-=-=-=-=-=-=-= arc03874 (plus_year)
arc03874=(OME_SCALAR)(currenttime(TestModelBm)==0.0||node03638==365.0 ? (OME_SCALAR)1.0 : (OME_SCALAR)0.0);

//=-=-=-=-=-=-=-=-=-= arc04234 (Add_RH)
arc04234=(OME_SCALAR)node03620;

//=-=-=-=-=-=-=-=-=-= arc04232 (remove_RH)
arc04232=(OME_SCALAR)var_delay((arc04234),((currenttime(TestModelBm)<node03682 ? (OME_SCALAR)1.0 : (OME_SCALAR)node03682)));

//=-=-=-=-=-=-=-=-=-= arc04241 (add_T)
arc04241=(OME_SCALAR)node03604;

//=-=-=-=-=-=-=-=-=-= arc04229 (remove_T)
arc04229=(OME_SCALAR)var_delay((arc04241),((currenttime(TestModelBm)<node03682 ? (OME_SCALAR)1.0 : (OME_SCALAR)node03682)));

//=-=-=-=-=-=-=-=-=-= node03648 (HeatIndex)
node03648.Evaluate(TestModelTime,TestModelBm);

//=-=-=-=-=-=-=-=-=-= node03718 (Heat_Index_in_C)
node03718=(OME_SCALAR)node03648*5.0/9.0-32.0;

//=-=-=-=-=-=-=-=-=-= node03680 (Init_RH)
node03680=(OME_SCALAR)node03682*66.2;

//=-=-=-=-=-=-=-=-=-= node03722 (EMS_measured)
node03722.Evaluate(TestModelTime,TestModelBm);
PROFILER_UPDATE(0);
PROFILER_OUTPUT(("/Users/pwing_000/Desktop/ShinyReports/Update" + std::to_string(TestModelTime ) + ".shiny").c_str());
AdjustTempPool();
}

