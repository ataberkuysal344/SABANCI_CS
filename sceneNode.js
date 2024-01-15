/**
 * @class SceneNode
 * @desc A SceneNode is a node in the scene graph.
 * @property {MeshDrawer} meshDrawer - The MeshDrawer object to draw
 * @property {TRS} trs - The TRS object to transform the MeshDrawer
 * @property {SceneNode} parent - The parent node
 * @property {Array} children - The children nodes
 */

class SceneNode {
    constructor(meshDrawer, trs, parent = null) {
        this.meshDrawer = meshDrawer;
        this.trs = trs;
        this.parent = parent;
        this.children = [];

        if (parent) {
            this.parent.__addChild(this);
        }
    }

    __addChild(node) {
        this.children.push(node);
    }

    draw(mvp, modelView, normalMatrix, modelMatrix) {
        /**
         * @Task1 : Implement the draw function for the SceneNode class.
         */
        
         // these are computing the local transformation matrix.
         var transformedMvp = MatrixMult(mvp, this.trs.getTransformationMatrix());
         var transformedModelView = MatrixMult(modelView, this.trs.getTransformationMatrix());
         var transformedNormals = MatrixMult(normalMatrix, this.trs.getTransformationMatrix());
         var transformedModel = MatrixMult(modelMatrix, this.trs.getTransformationMatrix());
 
 
      

        // Draw the MeshDrawer
        if (this.meshDrawer) {
            this.meshDrawer.draw(transformedMvp, transformedModelView, transformedNormals, transformedModel);
        }
        //i used the recursion for every child to be drawen
        this.children.forEach(child => child.draw(transformedMvp, transformedModelView, transformedNormals, transformedModel));
    }
    }

    

