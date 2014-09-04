#include "displacementfactory.h"
#include "ui_displacementfactory.h"

#include "types.h"

#include <iostream>

#include <QTableWidgetItem>

DisplacementFactory::DisplacementFactory(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DisplacementFactory),
    _project(NULL)
{
    ui->setupUi(this);
}

DisplacementFactory::~DisplacementFactory()
{
    delete ui;
}

void
DisplacementFactory::setProject(InputGen::Application::Project *p){
    _project = p;

    //todo: clear UI

    if ( p == NULL ){
        ;
    }else{
        // todo: load layers from the project

    }
}

void
DisplacementFactory::addLayerTriggerred(){
    std::cout << "addLayerTriggerred" << std::endl;

    typedef InputGen::Application::Scalar Scalar;
    typedef InputGen::Application::Primitive::vec vec;

    int ktype = ui->_displacementLayerAddCombo->currentIndex();

    if (ktype >= InputGen::DISPLACEMENT_KERNEL_TYPE::INVALID_KERNEL){
        std::cerr << "This should nerver happen... "
                  << __FILE__ << " "
                  << __LINE__ << std::endl;
        return;
    }

    int layerId = _project->nbDisplacementLayers();

    typedef InputGen::Application::Project::SampleContainer    SampleContainer;
    typedef InputGen::Application::Project::PrimitiveContainer PrimitiveContainer;

    InputGen::Application::Project::DisplacementKernel *kernel = NULL;

    switch(ktype){
    case InputGen::DISPLACEMENT_KERNEL_TYPE::DISPLACEMENT_RANDOM:
    {
        InputGen::RandomDisplacementKernel<Scalar,SampleContainer,PrimitiveContainer>* lkernel =
                new InputGen::RandomDisplacementKernel<Scalar,SampleContainer,PrimitiveContainer>;

        // set parameters

        // store for processing
        kernel = lkernel;
        break;
    }
    case InputGen::DISPLACEMENT_KERNEL_TYPE::DISPLACEMENT_BIAS:
    {
        InputGen::BiasDisplacementKernel<Scalar,SampleContainer,PrimitiveContainer>* lkernel =
                new InputGen::BiasDisplacementKernel<Scalar,SampleContainer,PrimitiveContainer>;

        // set parameters
        lkernel->bias = 0.2;

        // store for processing
        kernel = lkernel;
        break;
    }
    }


    // Update UI
    if (kernel != NULL)
    {
        // add layer and compute it
        _project->addDisplacementLayer(kernel);

        kernel->generateDisplacement( _project->displacementLayerPtr(layerId),
                                      _project->samples,
                                      _project->primitives);
        // add new row in the table
        int rowId = ui->_displacementLayerTable->rowCount();
        ui->_displacementLayerTable->setRowCount(rowId+1);

        // Row title
        QTableWidgetItem *_qtablewidgetitem1 = new QTableWidgetItem();
        _qtablewidgetitem1->setText(QString::number(rowId));
        ui->_displacementLayerTable->setVerticalHeaderItem(rowId, _qtablewidgetitem1);

        QTableWidgetItem *_qtablewidgetitem2 = new QTableWidgetItem();
        _qtablewidgetitem2->setText(QString::fromStdString(kernel->name));
        ui->_displacementLayerTable->setItem(rowId, 0, _qtablewidgetitem2);

        QTableWidgetItem *_qtablewidgetitem3 = new QTableWidgetItem();
        _qtablewidgetitem3->setCheckState(Qt::Checked);
        ui->_displacementLayerTable->setItem(rowId, 1, _qtablewidgetitem3);

        emit projectUpdated();
    }


}

void
DisplacementFactory::refreshFromView(){


    emit projectUpdated();
}

void
DisplacementFactory::recomputeDisplacement(){
    std::cout << "recomputeDisplacement()" << std::endl;

    if(_project == NULL) return;

    for(unsigned int i = 0; i<_project->nbDisplacementLayers(); i++)
        recomputeDisplacementLayer(i, false);

    // this signal must be emitted even without displacement layers,
    // to transmit the update information to other application elements
    emit projectUpdated();
}

void
DisplacementFactory::recomputeDisplacementLayer(int layerId, bool triggerSignal){
    std::cout << "recomputeDisplacementLayer()" << std::endl;

    if(_project == NULL) return;

    InputGen::Application::Project::DisplacementKernel *kernel = _project->displacementKernel(layerId);
    if(kernel == NULL) return;


    kernel->generateDisplacement( _project->displacementLayerPtr(layerId),
                                  _project->samples,
                                  _project->primitives);

    if(triggerSignal)
        emit projectUpdated();

}
