import React from 'react';
import axios from "axios";

class ChessBoard extends React.Component {
    constructor(props) {
        super(props);

        let chess = props.chess;
        if (chess == null) {
            chess = [
                [1, 1, 1, 1, 1, 1],
                [1, 1, 1, 1, 1, 1],
                [0, 0, 0, 0, 0, 0],
                [0, 0, 0, 0, 0, 0],
                [2, 2, 2, 2, 2, 2],
                [2, 2, 2, 2, 2, 2]
            ]
        }

        this.state = {
            sessionId: props.sessionId,
            availableMoves: null,
            chess: chess,
            blackTurn: true,
            from: null,
            to: null,
            waitingOpponent: false
        }

        if (this.state.sessionId == null && this.state.chess.length > 0) {
            axios.post(`${process.env.REACT_APP_BACKEND_URL}/move/session/register`, {
                rawBoard: this.state.chess,
                isPlayerTurn: this.state.blackTurn,
            }).then((response) => {
                response = response.data;
                if (response.code !== 0) {
                    console.error(response.message)
                } else {
                    const {sessionId, nextAvailableMoves} = response.data
                    nextAvailableMoves.forEach((e) => delete e['arc'])
                    console.info(`Set session ID to ${sessionId}`)
                    this.setState({
                        sessionId: sessionId,
                        availableMoves: nextAvailableMoves
                    })
                }
            })
        }
    }

    moveEquals(m1, m2) {
        return m1.from.x === m2.from.x && m1.from.y === m2.from.y && m1.to.x === m2.to.x && m1.to.y === m2.to.y
    }


    onChessClick = (x, y) => {
        // console.log(`state: ${JSON.stringify(this.state)}`)
        const from = this.state.from
        if (this.state.from == null) {
            console.info(`Set from ${x} ${y}`)
            this.setState({from: {x: x, y: y}})
        } else if (from.x === x && from.y === y) {
            console.info(`Unset from ${x} ${y}`)
            this.setState({from: null})
        } else {
            const move = {
                from: this.state.from,
                to: {x: x, y: y}
            }
            console.info(`Make move ${JSON.stringify(move)}`)
            if (!this.state.availableMoves.some((m) => this.moveEquals(m, move))) {
                console.warn(`Invalid move ${JSON.stringify(move)}`)
                this.setState({from: null})
                return;
            }
            const chess = this.state.chess;
            chess[move.to.y][move.to.x] = chess[move.from.y][move.from.x];
            chess[move.from.y][move.from.x] = 0;
            this.setState({
                chess: chess,
                waitingOpponent: true
            });
            axios.post(`${process.env.REACT_APP_BACKEND_URL}/session/${this.state.sessionId}/move`, {
                playerMove: move,
                doComputerMove: true,
            }).then((response) => {
                response = response.data;
                const {compMove, nextAvailableMoves} = response.data
                console.info(`Response move ${JSON.stringify(response.data)}`)
                if (compMove != null) {
                    chess[compMove.to.y][compMove.to.x] = chess[compMove.from.y][compMove.from.x];
                    chess[compMove.from.y][compMove.from.x] = 0;
                }
                nextAvailableMoves.forEach((e) => delete e['arc'])
                this.setState({
                    chess: chess,
                    availableMoves: nextAvailableMoves,
                    waitingOpponent: false
                })
            })
        }
    }

    shouldComponentUpdate(nextProps: Readonly<P>, nextState: Readonly<S>, nextContext: any): boolean {
        console.debug("Calling shouldComponentUpdate, nextState: " + nextState)
        return this.state.chess !== nextState.chess
            || this.state.blackTurn !== nextState.blackTurn
            || (this.state.from != null && nextState.from == null);
    }

    render() {
        console.debug("Chessboard rendering")

        return <div className="chess-board">
            {this.state.chess.flatMap((chessRow, i) => chessRow.map((type, j) =>
                <Chess onChessClick={this.onChessClick} size={0.7}
                       type={type} x={j} y={i} isSelected={false}
                       waitingOpponent={this.state.waitingOpponent}/>)
            )}
        </div>;
    }
}

const stateMap = {
    0: "empty", 1: "black", 2: "red"
}

class Chess extends React.Component {
    constructor(props) {
        super(props);
        console.log(`Chess constructor ${JSON.stringify(props)}`)

        this.state = {
            size: props.size,
            type: stateMap[props.type],
            isStressed: false,
            isSelected: props.isSelected,
            waitingOpponent: props.waitingOpponent,
            x: props.x,
            y: props.y
        }
    }

    static getDerivedStateFromProps(nextProps) {
        return {
            size: nextProps.size,
            isSelected: nextProps.isSelected,
            waitingOpponent: nextProps.waitingOpponent
        }
    }

    onClick = () => {
        console.debug(`Chess ${this.state.x} ${this.state.y} clicked`)
        this.props.onChessClick(this.state.x, this.state.y)
        if (!this.state.waitingOpponent) {
            this.setState({isSelected: !this.state.isSelected})
        }
    }

    onMoveOver = () => {
        console.debug(`Chess ${this.state.x} ${this.state.y} move over`)
        if (!this.state.isSelected) {
            this.setState({isStressed: true})
        }
    }

    onMoveOut = () => {
        console.debug(`Chess ${this.state.x} ${this.state.y} move out`)
        if (!this.state.isSelected) {
            this.setState({isStressed: false})
        }
    }

    render() {
        console.debug(`Chess rendering ${JSON.stringify(this.state)}`)
        // const xl = 0, xr = 0, yl = 0, yr = 0;
        const xl = -0.8, xr = -1.7, yl = -1, yr = -1;

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