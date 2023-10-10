import React from 'react';
import axios from "axios";

const EMPTY = 0;
const EMPTY_FROM = 3;
const EMPTY_TO = 4;
const PLAYER = 1;
const PLAYER_FROM = 5;
const PLAYER_TO = 6;
const OPPONENT = 2;
const OPPONENT_FROM = 7;
const OPPONENT_TO = 8;


class ChessBoard extends React.Component {
    constructor(props) {
        super(props);

        let chess = props.chess;
        if (chess == null) {
            chess = [
                [OPPONENT, OPPONENT, OPPONENT, OPPONENT, OPPONENT, OPPONENT],
                [OPPONENT, OPPONENT, OPPONENT, OPPONENT, OPPONENT, OPPONENT],
                [EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY],
                [EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY],
                [PLAYER, PLAYER, PLAYER, PLAYER, PLAYER, PLAYER],
                [PLAYER, PLAYER, PLAYER, PLAYER, PLAYER, PLAYER]
            ]
        }

        this.state = {
            sessionId: props.sessionId,
            availableMoves: null,
            chess: chess,
            blackTurn: true,
            from: null,
            to: null,
            compMove: null,
            waitingOpponent: false
        }

        if (this.state.sessionId == null && this.state.chess.length > 0) {
            axios.post(`${process.env.REACT_APP_BACKEND_URL}/session/register`, {
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
        if (this.state.waitingOpponent) {
            return
        }
        // console.log(`state: ${JSON.stringify(this.state)}`)
        const from = this.state.from
        const chess = this.state.chess
        if (from == null) {
            if (this.state.chess[x][y] !== PLAYER) {
                console.info(`Skip from ${x} ${y}`)
                return;
            }
            console.info(`Set from ${x} ${y}`)
            chess[x][y] = PLAYER_FROM;
            this.setState({from: {x: x, y: y}, chess: chess})
        } else if (from.x === x && from.y === y) {
            console.info(`Unset from ${x} ${y}`)
            chess[x][y] = PLAYER;
            this.setState({from: null, chess: chess})
        } else {
            if (this.state.compMove != null) {
                chess[this.state.compMove.from.x][this.state.compMove.from.y] = EMPTY;
                chess[this.state.compMove.to.x][this.state.compMove.to.y] = OPPONENT;
                this.setState({compMove: null, chess: chess})
            }

            const move = {
                from: from,
                to: {x: x, y: y}
            }
            console.info(`Make move ${JSON.stringify(move)}`)
            if (!this.state.availableMoves.some((m) => this.moveEquals(m, move))) {
                console.warn(`Invalid move ${JSON.stringify(move)} ${JSON.stringify(this.state.availableMoves)}`)
                chess[from.x][from.y] = PLAYER;
                this.setState({from: null, chess: chess})
                return;
            }
            chess[move.from.x][move.from.y] = PLAYER_FROM;
            if (chess[move.to.x][move.to.y] === EMPTY) {
                chess[move.to.x][move.to.y] = EMPTY_TO;
            } else if (chess[move.to.x][move.to.y] === OPPONENT) {
                chess[move.to.x][move.to.y] = OPPONENT_TO;
            }
            this.setState({
                chess: chess,
                from: null,
                waitingOpponent: true
            });

            axios.post(`${process.env.REACT_APP_BACKEND_URL}/session/${this.state.sessionId}/move`, {
                playerMove: move,
                doComputerMove: true,
            }).then((response) => {
                response = response.data;
                const {compMove, nextAvailableMoves} = response.data
                console.info(`Response move ${JSON.stringify(response.data)}`)
                chess[move.from.x][move.from.y] = EMPTY;
                chess[move.to.x][move.to.y] = PLAYER
                if (compMove != null) {
                    chess[compMove.from.x][compMove.from.y] = EMPTY_FROM;
                    chess[compMove.to.x][compMove.to.y] = OPPONENT_TO;
                }
                nextAvailableMoves.forEach((e) => delete e['arc'])
                this.setState({
                    chess: chess,
                    compMove: compMove,
                    availableMoves: nextAvailableMoves,
                    waitingOpponent: false
                })
            })
        }
    }

    render() {
        console.debug(`Chessboard rendering chess=${this.state.chess}`)

        return <div className="chess-board">
            {this.state.chess.flatMap((chessRow, i) => chessRow.map((type, j) =>
                <Chess onChessClick={this.onChessClick} size={0.7}
                       type={type} x={i} y={j} isSelected={false}
                       waitingOpponent={this.state.waitingOpponent}/>)
            )}
        </div>;
    }
}

class Chess extends React.Component {
    constructor(props) {
        super(props);
        console.log(`Chess constructor ${JSON.stringify(props)}`)

        this.state = {
            size: props.size,
            type: props.type,
            isStressed: false,
            isSelected: props.isSelected,
            waitingOpponent: props.waitingOpponent,
            x: props.x,
            y: props.y
        }
    }

    static getDerivedStateFromProps(nextProps) {
        return {
            type: nextProps.type,
        }
    }

    onClick = () => {
        console.debug(`Chess ${this.state.x} ${this.state.y} clicked`)
        this.props.onChessClick(this.state.x, this.state.y)
    }

    onMoveOver = () => {
        console.debug(`Chess ${this.state.x} ${this.state.y} move over`)
        this.setState({isStressed: true})
    }

    onMoveOut = () => {
        console.debug(`Chess ${this.state.x} ${this.state.y} move out`)
        this.setState({isStressed: false})
    }

    render() {
        const yl = -0.8, yr = -1.7, xl = -1, xr = -1;

        let realSize = 0.85;
        let style = {}
        switch (this.state.type) {
            case PLAYER:
                style = {backgroundColor: "blue"};
                realSize = this.state.isStressed ? 0.85 : 0.7;
                break;
            case PLAYER_FROM:
                style = {backgroundColor: "blue", board: "5px dashed greenyellow"};
                break;
            case PLAYER_TO:
                style = {backgroundColor: "blue", board: "5px solid red"};
                break;
            case OPPONENT:
                style = {backgroundColor: "yellow"};
                realSize = this.state.isStressed ? 0.85 : 0.7;
                break;
            case OPPONENT_FROM:
                style = {backgroundColor: "yellow", board: "5px dashed greenyellow"};
                break;
            case OPPONENT_TO:
                style = {backgroundColor: "yellow", board: "5px solid red"};
                break;
            case EMPTY:
                style = {backgroundColor: "gray", opacity: this.state.isStressed ? 0.75: 0};
                realSize = 0.7;
                break;
            case EMPTY_FROM:
                style = {backgroundColor: "gray", opacity: 0.75, board: "5px dashed greenyellow"};
                realSize = 0.7;
                break;
            case EMPTY_TO:
                style = {backgroundColor: "gray", opacity: 0.75, board: "5px solid red"};
                realSize = 0.7;
                break;
            default:  break;
        }

        style = {...style,
            position: 'absolute',
            top: "" + ((this.state.x + 2 - realSize / 2) / 9 * (100 - xl - xr) + xl) + "%",
            left: "" + ((this.state.y + 2 - realSize / 2) / 9 * (100 - yl - yr) + yl) + "%",
            height: "" + (realSize / 9 * 100) + "%",
            width: "" + (realSize / 9 * 100) + "%",
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