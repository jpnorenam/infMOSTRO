# infMOSTRO

## Instalación

### Dependencias
`sudo apt install gcc g++ make cmake pkg-config`\
`sudo apt libjsoncpp-dev libboost-math-dev`

### Compilación de la librería
`git clone git@github.com:jpnorenam/infMOSTRO.git`\
`cd infMOSTRO`\
`mkdir build && cd build`\
`cmake ..`\
`make -j $(nproc)`\
`sudo make install`\
`ldconfig`

### Añadir variable de entorno

`echo "export INFMOSTRO_HOME=/usr/local/etc/infmostro" >> ~/.bash_profile`\
`source ~/.bashrc`

## Copyright

2020, Laboratorio de Gestión de Sistemas en Tiempo Real, Facultad de Minas, Universidad Nacional de Colombia

## Contacto

[![LGSTR Logo](docs/LGSTR_logo.png)](https://sites.google.com/unal.edu.co/lab-gstr/)

- Juan Pablo Noreña <jpnorenam@unal.edu.co>
- Andrés Felipe Acosta <afacostag@unal.edu.co>
- Andrés Osorio <anosoriosa@unal.edu.co>

[Laboratorio de Gestión de Sistemas en Tiempo Real](https://sites.google.com/unal.edu.co/lab-gstr/) \
[Facultad de Minas](https://minas.medellin.unal.edu.co/) \
[Universidad Nacional de Colombia](https://unal.edu.co/)
