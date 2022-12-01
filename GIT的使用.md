# GIT的使用
## 2021/3/20  23：24
1. 现在一个非C盘里创建一个文件夹（最好是新建的）以此来作为本地仓库吧
2. 然后在文件夹中右键点击“git bash here”,在文件夹中创建了一个隐藏文件夹“.git”
3. 在github上面打开自己账户的setting，点击ssh，第一次用的话是空的要新建一个密钥，标题可以自己命名，key需要在自己电脑里面找（相当于自己电脑的地址）
4. ![image-20210329233800863](https://gitee.com/mrsu18/pictures-of-the-warehouse/raw/master/image/image-20210329233800863.png)
-  (第一次使用没有ssh文件的话则需要这样操作)
-  文件夹的位置在C->用户->第一个文件夹->.ssh,用记事本打开id-rsa.pub
5. ![image-20210329233625304](https://gitee.com/mrsu18/pictures-of-the-warehouse/raw/master/img/20210330224959.png)（把一大串地址复制到key里面)

![image-20210329233457864](https://gitee.com/mrsu18/pictures-of-the-warehouse/raw/master/img/20210330131102.png)
在github上面新建一个远程仓库

![image-20210329234142319](https://gitee.com/mrsu18/pictures-of-the-warehouse/raw/master/img/20210330131104.png)

![image-20210329234152097](https://gitee.com/mrsu18/pictures-of-the-warehouse/raw/master/img/20210330131107.png)

这里初始化仓库的时候什么都别勾选，否则会出错

![image-20210329234205681](https://gitee.com/mrsu18/pictures-of-the-warehouse/raw/master/img/20210330131110.png)
然后复制ssh地址
再回到git
输入git remote add origin 粘贴地址
回车不报错就成功链接了

然后第一次推送项目（从本地到远程
用git push -u origin master回车
然后就全部搞定了
以后添加项目就三步
1.git add xxx
2.git commit -m “xxx”
3.git push



删除文件代码：git delet xxx
           git commit -m""(这个相当于记录了你干嘛了)
           git push 把它push到远程参考
           
版本的回退



git remote rm origin 删除远程git仓库

-   在你使用完版本回退之后，你再上传文件然后会发现无法git push,这时候可能是远程代码跟本地有某些冲突不可忽略

    所以需要git pull之后再git push，会这样

    ![image-20210611001422123](https://gitee.com/mrsu18/pictures-of-the-warehouse/raw/master/image/image-20210611001422123.png)
