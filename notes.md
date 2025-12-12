# Pipeline stages
### IVRFB
1. ***I***sn't : ***I***nput stage (fixed)
2. ***V***ulkan : ***V***ertex shaders (programable)
3. ***R***eally : ***R***atserize triangles (fixed)
4. ***F***uckin : ***F***ragment Shader  (programable)
5. ***B***onkers : ***B***lend colors (fixed)


# space matrix mults for game obj verts
1. model space
2. world
3. camera
4. view aka *Canonical View Volume* aka *Normalized Device Coordinate* (NDC) Space.
5. screen



## **Q:** is the "view  matrix" the cameras frustrum?
### **A:** no, "view" is the NDC Space

## **Q:** how does this `GameObject` constructor increment the game object `id`?

            static M4GameObject createGameObject(){
                static id_t currentId = 0;
                return M4GameObject{currentId++};
            }
## **Q:** 
