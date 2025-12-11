from setuptools import find_packages, setup

package_name = 'pkg02_helloworld_py' # 包名称

setup( # 配置包信息
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),  # 排除测试目录
    data_files=[
        ('share/ament_index/resource_index/packages', 
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],  # 安装依赖
    zip_safe=True,
    maintainer='chiway',
    maintainer_email='chiway@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            'helloworld = pkg02_helloworld_py.helloworld:main'
        ],
    },
)
