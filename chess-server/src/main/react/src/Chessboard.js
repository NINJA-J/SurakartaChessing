import React from 'react';
import background from "./img/surakarta2.jpg"

class ChessBoard extends React.Component {
    constructor(prop) {
        super();
        this.state = {
            board: prop.board,
            blackTurn: prop.blackTurn
        }
    }

    onChessClick(chess: Chess) {
        return true
    }

    render() {
        let items = []
        for (let i = 0; i < 6; i++) {
            for (let j = 0; j < 6; j++) {
                items.push(<Chess onChessClick={this.onChessClick} size={0.7}
                                  type={i < 2 ? "red" : (i < 4 ? "empty" : "black")}
                                  x={j} y={i}/>)
            }
        }
        return <div style={{display: "flex"}}>
            <div style={{
                width: "30%"
            }}></div>
            <div className="chess-board"
                 style={{
                     width: "40%",
                     paddingTop: "40%",
                     borderStyle: "solid",
                     borderColor: "black",
                     // backgroundImage: `url("../public/logo512.png")`,
                     backgroundImage: `url(${background})`,
                     backgroundSize: "cover",
                     // display: "flex",
                     position: "relative",
                 }}>
                {items}
            </div>
            <div style={{
                width: "30%"
            }}></div>
        </div>;
    }
}

class Chess extends React.Component {
    constructor(prop) {
        super(prop);

        this.state = {
            size: prop.size,
            type: prop.type,
            isStressed: false,
            isSelected: false,
            x: prop.x,
            y: prop.y
        }
    }

    onClick = () => {
        // console.log(this.state)
        if (this.props.onChessClick(this)) {
            this.setState({isSelected: !this.state.isSelected})
        }
    }

    onMoveOver = () => {
        // console.log(this.state)
        if (!this.state.isSelected) {
            this.setState({isStressed: true})
        }
    }

    onMoveOut = () => {
        // console.log(this.state)
        if (!this.state.isSelected) {
            this.setState({isStressed: false})
        }
    }

    render() {
        // const xl = 0, xr = 0, yl = 0, yr = 0;
        const xl=-0.8, xr=-1.7, yl=-1, yr=-1;

        const realSize = (this.state.isSelected || this.state.isStressed) && this.state.type !== "empty" ?
            (this.state.size / 2 + 0.5) : this.state.size
        const style = {
            position: 'absolute',
            top: "" + ((this.state.y + 2 - realSize / 2) / 9 * (100 - yl - yr) + yl) + "%",
            left: "" + ((this.state.x + 2 - realSize / 2) / 9 * (100 - xl - xr) + xl) + "%",
            height: "" + (realSize / 9 * 100) + "%",
            width: "" + (realSize / 9 * 100) + "%",
        }
        // console.log(style)
        if (this.state.type === "empty") {
            style.backgroundColor = "gray"
            style.opacity = this.state.isStressed ? 0.75 : 0.0
        } else {
            style.backgroundColor = this.state.type === "black" ? "blue" : "yellow"
        }

        return <div className="chess"
                    style={style}
                    onClick={this.onClick}
                    onMouseOver={this.onMoveOver}
                    onMouseOut={this.onMoveOut}>
        </div>;
    }
}

export default ChessBoard;